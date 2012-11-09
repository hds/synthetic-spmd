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
	SSPeers		*peers;
	SSWorkArray	*work_array;

	MPI_Init(&argc,&argv);

	config = initAppConfig(argc, argv);
	if (config)  {

		work_array = workArrayInitWithLength(config->wunits);
		workArrayFillUnits(work_array, config->wunit_weight[0], config->wunit_weight[1]);
		//workArrayPrintUnits(work_array);


		peers = peersInit(config->dims, config->mpi_rank);

		applicationLoop(config, peers, work_array);

		workArrayRelease(work_array);
	}
	    
	MPI_Finalize();

	return 0;
}

void applicationLoop(SSAppConfig *config, SSPeers *peers, SSWorkArray *work_array)
{
	unsigned int	i;
	SSTInterval	t1, t2;
	SSWorkMatrices	matrices;
	
	matrices = workMatricesInit(WORK_MATRIX_SIZE);

	for (i = 0; i < config->iterations; i++)  {
		
		// Barrier
		t1 = getCurrentTime();
		barrier(config);
		t2 = getCurrentTime();
		printf("[%3d] iter: %d, barrier: %ld us\n", config->mpi_rank, i, t2-t1);
		
		// Work
		t1 = getCurrentTime();
		work(work_array, matrices);
		t2 = getCurrentTime();
		printf("[%3d] iter: %d, work: %ld us\n", config->mpi_rank, i, t2-t1);

		// Communication
		t1 = t2;
		communication(peers, config->comm_weight, config->mpi_rank);
		t2 = getCurrentTime();
		printf("[%3d] iter: %d, comm: %ld us\n", config->mpi_rank, i, t2-t1);
	}


	workMatricesRelease(&matrices);
}

void barrier(SSAppConfig *config)
{
	// This may need to be more complex later on, but for now it's
	// just an MPI_Barrier call.

	MPI_Barrier(MPI_COMM_WORLD);
} // barrier()

void work(SSWorkArray *work_array, SSWorkMatrices matrices)
{
	unsigned int	u, w;


	for (u = 0; u < work_array->length; u++)  {
		for (w = 0; w < work_array->elements[u].weight; w++)  {
			workMatricesMultiply(matrices);
		}
	}

} // work()

void communication(SSPeers *peers, unsigned int comm_weight, int rank)
{
	peerCommunication(peers, comm_weight, rank);
} // communication ()

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
		if (strcmp(argv[i], "-u") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			config->wunits = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-w") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			if (sscanf(argv[++i], "%d,%d", &(config->wunit_weight[0]), &(config->wunit_weight[1])) != 2)  {
				fprintf(stderr, "error: Work unit weight range must be specified min,max (they were %s)\n", argv[i]);
				return displayUsageAndReleaseConfig(config);
			}
			if (config->wunit_weight[0] > config->wunit_weight[1])  {

				fprintf(stderr, "error: work unit weight min (%d) must be smaller than or equal to max (%d)\n", config->wunit_weight[0], config->wunit_weight[1]);
				return displayUsageAndReleaseConfig(config);
			}
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

SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config)
{
	fprintf(stderr, "usage: synthetic-spmd [-v] -d WxH [-u work_units] [-w work_weight_min,work_weight_max] [-c comm_weight]\n\n");

	releaseAppConfig(config);
	config = NULL;

	return config;
} // displayUsage()

void releaseAppConfig(SSAppConfig *config)
{
	if (config != NULL)
		free(config);
} // releaseAppConfig()

