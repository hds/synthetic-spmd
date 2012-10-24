#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "synthetic-spmd.h"

int main(int argc, char **argv)
{
	int 		mpi_rank, mpi_size;
	SSAppConfig	*config;
	SSPeers		peers;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

	config = initAppConfig(argc, argv, mpi_size);
	if (config)  {

		//printf("Hello World from Node %d of %d\n", mpi_rank, mpi_size);

//		unsigned int	dims[SPMD_GRID_DIMS];
//		dims[0] = 2;
//		dims[1] = 2;
		peers = initPeers(config->dims, mpi_rank);

		peerCommunication(&peers, mpi_rank);
	}
	    
	MPI_Finalize();

	return 0;
}

void work(SSWorkUnit *work_units)
{
	// Do work
}

void peerCommunication(SSPeers *peers, int rank)
{
	unsigned int 	i;
	int		local_data, recv_data;
	MPI_Request	req;
	MPI_Status	status;
	local_data = rank;

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		MPI_Isend(&local_data,		// buf
		          1,			// count
			  MPI_INT,		// datatype
			  peers->ids[i],	// dest
			  123,			// tag
		          MPI_COMM_WORLD,	// communicator
			  &req);		// request
		MPI_Request_free(&req);
		printf("[%d] Sent %d to neighbour %d (%d)\n", rank, local_data, i, peers->ids[i]);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		//printf("[%d] ready to receive from neighbour %d (%d)\n", rank, i, peers->ids[i]);
		MPI_Recv(&recv_data,		// buf
		         1,			// count (max)
			 MPI_INT,		// datatype
			 peers->ids[i],		// source
			 123,			// tag
			 MPI_COMM_WORLD,	// communicator
			 &status);		// status
		printf("[%d] Received %d from neighbour %d (%d)\n", rank, recv_data, i, peers->ids[i]);
	}


}

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

SSAppConfig *displayUsageAndFreeConfig(SSAppConfig *config)  {
	fprintf(stderr, "usage: synthetic-spmd [-v] [-w work_units]\n\n");

	releaseAppConfig(config);
	config = NULL;

	return config;
} // displayUsage()

SSAppConfig *initAppConfig(int argc, char **argv, int mpi_size)
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
	config->wunit_weight[1] = 0;
	config->comm_weight = 0;
	config->verbose = 0;

	for (i = 1; i < argc; i++)  {
		if (strcmp(argv[i], "-w") == 0)  {
			if (argc <= i+1)
				return displayUsageAndFreeConfig(config);
			config->wunits = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-d") == 0)  {
			if (argc <= i+1)
				return displayUsageAndFreeConfig(config);
			if (sscanf(argv[++i], "%dx%d", &(config->dims[0]), &(config->dims[1])) != 2)  {
				fprintf(stderr, "error: Dimensions must be in the format <int>x<int> (they were %s)\n", argv[i]);
				return displayUsageAndFreeConfig(config);
			}
			if (config->dims[0] * config->dims[1] != mpi_size)  {
				fprintf(stderr, "error: Dimensions %d x %d do not equal MPI size %d.\n", config->dims[0], config->dims[1], mpi_size);
				return displayUsageAndFreeConfig(config);
			}
		}
		else if (strcmp(argv[i], "-v") == 0)  {
			config->verbose = 1;
		}
	}

	return config;
} // initAppConfig()

void releaseAppConfig(SSAppConfig *config)
{
	if (config != NULL)
		free(config);
} // releaseAppConfig()

//SSPeersRef ssPeersCreate()
//{
//	SSPeersRef	peers = NULL;
//
//	if ((peers = malloc(sizeof(struct __SSPeers))) == NULL)  {
//		fprintf(stderr, "Could not create instance of SSPeersRef, out of memory\n");
//		return peers;
//	}
//
//	return peers;
//}
//
//void ssPeersRelease(SSPeersRef peers)
//{
//	if (peers != NULL)
//		free(peers);
//}
