#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#include "synthetic-spmd.h"
#include "ss-timing.h"

int main(int argc, char **argv)
{
	SSAppConfig	*config;
	SSPeers		peers;
	SSWorkMatrices	matrices;

	MPI_Init(&argc,&argv);

	config = initAppConfig(argc, argv);
	if (config)  {

		peers = initPeers(config->dims, config->mpi_rank);
		matrices = workMatricesInit(WORK_MATRIX_SIZE);

		applicationLoop(config, peers, matrices);

		workMatricesRelease(&matrices);
	}
	    
	MPI_Finalize();

	return 0;
}

void applicationLoop(SSAppConfig *config, SSPeers peers, SSWorkMatrices matrices)
{
	unsigned int	i;
	SSTInterval	t1, t2;


	for (i = 0; i < config->iterations; i++)  {

		//workMatricesPrint(matrices);
		
		
		t1 = getCurrentTime();
		work(NULL, matrices);
		t2 = getCurrentTime();
		printf("[%3d] iter: %d, work: %ld us\n", config->mpi_rank, i, t2-t1);

		t1 = t2;
		peerCommunication(&peers, config->comm_weight, config->mpi_rank);
		t2 = getCurrentTime();
		printf("[%3d] iter: %d, comm: %ld us\n", config->mpi_rank, i, t2-t1);


	}
}

void work(SSWorkUnit *work_units, SSWorkMatrices matrices)
{
	unsigned int	i, j, k;

	// Do work
	for (j = 0; j < matrices.n; j++)  {
		for (i = 0; i < matrices.n; i++)  {
			// j-th row of m1 x i-th column of m2
			matrices.r[j*matrices.n + i] = 0;
			for (k = 0; k < matrices.n; k++)  {
				matrices.r[j*matrices.n + i] += matrices.m1[j*matrices.n + k] * matrices.m2[k*matrices.n + i];
			}
		}
	}


}

void peerCommunication(SSPeers *peers, unsigned int comm_weight, int rank)
{
	unsigned int 	i;
	//int		local_data, recv_data;
	SSCommData	*send, *recv;
	MPI_Request	req;
	MPI_Status	status;
	//local_data = rank;

	// Sanity check
	if (comm_weight == 0)
		return;

	send = initCommData(comm_weight, SSCommDataInitActionZero);
	recv = initCommData(comm_weight, SSCommDataInitActionNone);

	if (send == NULL || recv == NULL)  {
		fprintf(stderr, "Sending or receiving data couldn't be allocated.\n");
		return;
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		MPI_Isend(send->data,		// buf
		          send->n,		// count
			  MPI_UNSIGNED_CHAR,	// datatype
			  peers->ids[i],	// dest
			  123,			// tag
		          MPI_COMM_WORLD,	// communicator
			  &req);		// request
		MPI_Request_free(&req);
		//printf("[%d] Sent %d to neighbour %d (%d)\n", rank, local_data, i, peers->ids[i]);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		//printf("[%d] ready to receive from neighbour %d (%d)\n", rank, i, peers->ids[i]);
		MPI_Recv(recv->data,		// buf
		         recv->n,		// count (max)
			 MPI_UNSIGNED_CHAR,	// datatype
			 peers->ids[i],		// source
			 123,			// tag
			 MPI_COMM_WORLD,	// communicator
			 &status);		// status
		//printf("[%d] Received %d from neighbour %d (%d)\n", rank, recv_data, i, peers->ids[i]);
	}

	releaseCommData(send);
	releaseCommData(recv);


}

SSAppConfig *initAppConfig(int argc, char **argv)
{
	unsigned int	i;
	SSAppConfig	*config = NULL;

	if ((config = (SSAppConfig *)malloc(sizeof(SSAppConfig))) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSAppConfig.\n");
		return config;
	}
	
	// Default values
	config->dims[0] = 0;
	config->dims[1] = 0;
	config->wunits = 0;
	config->wunit_weight[0] = 0;
	config->wunit_weight[1] = 1;
	config->comm_weight = 0;
	config->iterations = 10;
	config->verbose = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &(config->mpi_rank));
	MPI_Comm_size(MPI_COMM_WORLD, &(config->mpi_size));

	

	for (i = 1; i < argc; i++)  {
		if (strcmp(argv[i], "-w") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			config->wunits = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-d") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			if (sscanf(argv[++i], "%dx%d", &(config->dims[0]), &(config->dims[1])) != 2)  {
				fprintf(stderr, "error: Dimensions must be in the format <int>x<int> (they were %s)\n", argv[i]);
				return displayUsageAndReleaseConfig(config);
			}
			if (config->dims[0] * config->dims[1] != config->mpi_size)  {
				fprintf(stderr, "error: Dimensions %d x %d do not equal MPI size %d.\n", config->dims[0], config->dims[1], config->mpi_size);
				return displayUsageAndReleaseConfig(config);
			}
		}
		else if (strcmp(argv[i], "-c") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			config->comm_weight = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-v") == 0)  {
			config->verbose = 1;
		}
	}

	return config;
} // initAppConfig()

SSPeers initPeers(unsigned int *dims, int rank)
{
	SSPeers		peers;
	peers.n = 0;

	if (SPMD_GRID_DIMS != 2)  {
		fprintf(stderr, "Only 2 dimensional grids currently supported\n");
		return peers;
	}
	unsigned int	x, y;
	unsigned int	i;
	unsigned int	px, py;

	x = rank % dims[0]; // dims[0] is the grid width
	y = rank / dims[0];

	for (i = 0; i < MAX_PEER_COUNT; i++)
		peers.ids[i] = -1;

	// North peer
	if ((y > 0) || (GRID_WRAP && dims[1] > 2) || (FORCE_GRID_WRAP))  {
		px = x;
		py = (y-1+dims[1]) % dims[1];
		peers.ids[0] = (py*dims[0]) + px;
	}

	// East peer
	if ((x < dims[0]-1) || (GRID_WRAP && dims[0] > 2) || (FORCE_GRID_WRAP))  {
		px = (x+1) % dims[0];
		py = y;
		peers.ids[1] = (py*dims[0]) + px;
	}
	
	// South peer	
	if ((y < dims[1]-1) || (GRID_WRAP && dims[1] > 2) || (FORCE_GRID_WRAP))  {
		px = x;
		py = (y+1) % dims[1];
		peers.ids[2] = (py*dims[0]) + px;
	}

	// West peer
	if ((x > 0) || (GRID_WRAP && dims[0] > 2) || (FORCE_GRID_WRAP))  {
		px = (x-1+dims[0]) % dims[0];
		py = y;
		peers.ids[3] = (py*dims[0]) + px;
	}

	peers.n = 4;
//	printf("[%d] I have coords %d,%d\n", rank, x, y);
//	printf("    [%d] neigh: %d, %d, %d, %d\n", rank, peers.ids[0], peers.ids[1], peers.ids[2], peers.ids[3]);

	return peers;
}

SSCommData *initCommData(unsigned int weight, SSCommDataInitAction init_action)
{
	SSCommData	*cdata = NULL;
	unsigned int	i;

	if ((cdata = (SSCommData*)malloc(sizeof(SSCommData))) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSCommData structure.\n");
		return cdata;
	}

	cdata->n = weight;
	cdata->data = NULL;
	if ((cdata->data = (unsigned char *)malloc(sizeof(unsigned char)*cdata->n)) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSCommData data.\n");
		free(cdata);
		cdata = NULL;
		return cdata;
	}

	if (init_action == SSCommDataInitActionNone)  {
		// Nothing.
	}
	else if (init_action == SSCommDataInitActionZero)  {
		for (i = 0; i < cdata->n; i++)  {
			cdata->data[i] = (unsigned char)(0);
		}
	}
	else if (init_action == SSCommDataInitActionRandom)  {
		for (i = 0; i < cdata->n; i++)  {
			cdata->data[i] = (unsigned char)(random()%sizeof(unsigned char));
		}
	}

	return cdata;
} // initCommData()

void releaseCommData(SSCommData *cdata)
{
	if (cdata->data != NULL)
		free(cdata->data);
	cdata->data = NULL;

	if (cdata != NULL)
		free(cdata);
} // releaseCommData()
SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config)
{
	fprintf(stderr, "usage: synthetic-spmd [-v] -d WxH [-w work_units] [-c comm_weight]\n\n");

	releaseAppConfig(config);
	config = NULL;

	return config;
} // displayUsage()

void releaseAppConfig(SSAppConfig *config)
{
	if (config != NULL)
		free(config);
} // releaseAppConfig()

