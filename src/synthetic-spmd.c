#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "synthetic-spmd.h"
#include "ss-migration.h"

// Global variables containing the number of work units to send to each
// neighbour during the next migration.
static int next_movement_north;
static int next_movement_east;
static int next_movement_south;
static int next_movement_west;

static int work_iteration;

int TEST=2;


void Insert_DMLib(){
	fprintf(stdout, "Insert_DMLib\n");
	int a = 2;
}

void applicationLoop(SSAppConfig *config, SSPeers *peers, SSWorkArray *work_array)
{
	unsigned int	i, j;
	SSTInterval	t1, t2;
	SSWorkMatrices	matrices;
	int		movement[MAX_PEER_COUNT];
	
	matrices = workMatricesInit(WORK_MATRIX_SIZE);

	for (i = 0; i < config->iterations; i++)  {
		
		// Barrier
		t1 = getCurrentTime();
		barrier(i, config);
		t2 = getCurrentTime();
		outputElapsedTime(i, config->mpi_rank, t2-t1, SSBadgeBarrier);
		//printf("[%3d] iter: %d, barrier: %ld us\n", config->mpi_rank, i, t2-t1);
		
		// Work
		t1 = getCurrentTime();
		int array_length = work_array->length;
		work(i, config->mpi_rank, work_array, array_length, matrices);
		t2 = getCurrentTime();
		outputElapsedTime(i, config->mpi_rank, t2-t1, SSBadgeWork);
		//printf("[%3d] iter: %d, work: %ld us\n", config->mpi_rank, i, t2-t1);

		// Communication
		t1 = t2;
		communication(i, peers, config->comm_weight, config->mpi_rank);
		t2 = getCurrentTime();
		outputElapsedTime(i, config->mpi_rank, t2-t1, SSBadgeCommunication);
		//printf("[%3d] iter: %d, comm: %ld us\n", config->mpi_rank, i, t2-t1);

		// Migration
		if ((config->migration_freq > 0) &&
			(i % config->migration_freq  == config->migration_freq - 1))  {
			t1 = t2;
			movement[0] = next_movement_north;
			movement[1] = next_movement_east;
			movement[2] = next_movement_south;
			movement[3] = next_movement_west;
			
			printf("%d\t%d\t[%d, %d, %d, %d]\n", i, config->mpi_rank, movement[0], movement[1], movement[2], movement[3]);

			workUnitMigration(peers, work_array, movement, i);
			t2 = getCurrentTime();
			outputElapsedTime(i, config->mpi_rank, t2-t1, SSBadgeMigration);
			printf("%d\t%d\tUnits of Work: %d -> %d\n", i, config->mpi_rank, array_length, work_array->length);

			next_movement_north = 0;
			next_movement_east = 0;
			next_movement_south = 0;
			next_movement_west = 0;
			//printf("[%3d] iter: %d, migration: %ld", config->mpi_rank, i, t2-t1);
			//workArrayPrintUnits(work_array);
		}
	}


	workMatricesRelease(&matrices);
} // applicationLoop()

void barrier(int iteration, SSAppConfig *config)
{
	// This may need to be more complex later on, but for now it's
	// just an MPI_Barrier call.

	MPI_Barrier(MPI_COMM_WORLD);
} // barrier()


void work(int iteration, int mpi_rank, SSWorkArray *work_array, int work_array_length, SSWorkMatrices matrices)
{
	unsigned int	u, w;
	char		prefix[40];
	//printf("%d\t%d\tBefore assign work_iteration\n", iteration, mpi_rank);
	work_iteration = iteration;

	sprintf(prefix, "%d\t%d\tWork to do ", iteration, mpi_rank);
	workArrayPrintUnitWithPrefix(work_array, prefix);
	
	//printf("%d\t%d\tBefore outer loop\n", iteration, mpi_rank);
	for (u = 0; u < work_array->length; u++)  {
		for (w = 0; w < work_array->elements[u].weight; w++)  {
			workMatricesMultiply(matrices);
		}
	}

	//printf("%d\t%d\tEnd of work()\n", iteration, mpi_rank);

} // work()

void communication(int interation, SSPeers *peers, unsigned int comm_weight, int rank)
{
	peerCommunication(peers, comm_weight, rank);
} // communication ()

void outputElapsedTime(unsigned int iteration, int mpi_rank, SSTInterval elapsed, char *action)
{
	fprintf(stdout, "%u\t%d\t%ld\t%s\n", iteration, mpi_rank, elapsed, action);
} // outputElapsedTime()

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
	config->wunits = 20;
	config->wunit_weight[0] = 40;
	config->wunit_weight[1] = 40;
	config->comm_weight = 1;
	config->iterations = 10;
	config->migration_freq = 1;
	config->verbose = 0;

	next_movement_north = 0;
	next_movement_east = 0;
	next_movement_south = 0;
	next_movement_west = 0;

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
		else if (strcmp(argv[i], "-i") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			config->iterations = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-m") == 0)  {
			if (argc <= i+1)
				return displayUsageAndReleaseConfig(config);
			config->migration_freq = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-v") == 0)  {
			config->verbose = 1;
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)  {
			displayUsageAndReleaseConfig(config);
		}
	}

	fprintf(stdout, "Soy la sintetica al final de initAppConfig\n");

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

int main(int argc, char **argv)
{
	fprintf(stdout, "Soy la sintetica!!! \n");
	SSAppConfig	*config;
	SSPeers		*peers;
	SSWorkArray	*work_array;

	MPI_Init(&argc, &argv);
	fprintf(stdout, "argv[0]=%s\n", argv[0]);
	fprintf(stdout, "argv[1]=%s\n", argv[1]);
	fprintf(stdout, "argv[2]=%s\n", argv[2]);
	config = initAppConfig(argc, argv);
	Insert_DMLib();
	int a = TEST;
	if (config)  {
//		fprintf(stdout, "Soy la sintetica dentro de if(config)\n");
		peers = peersInit(config->dims, config->mpi_rank);
		if (peersRealPeerCount(peers) < 4)  {
			// Add ~20% extra work units if we're on a border.
			config->wunits += (config->wunits / 5);
		}

		work_array = workArrayInitWithLength(config->wunits);
//		fprintf(stdout, "Soy la sintetica despues de workArrayInit \n");
		workArrayFillUnits(work_array, config->wunit_weight[0], config->wunit_weight[1]);
		//workArrayPrintUnits(work_array);
//		fprintf(stdout, "Soy la sintetica despues de workArrayFill \n");
//		fprintf(stdout, "Valor de TEST=%d\n", TEST);



//		fprintf(stdout, "Soy la sintetica despues de peersInit \n");
		applicationLoop(config, peers, work_array);
		fprintf(stdout, "Soy la sintetica despues de applicationLoop \n");
		workArrayRelease(work_array);
		fprintf(stdout, "Soy la sintetica despues de workArrayRelease \n");
	}
	    
	MPI_Finalize();

	return 0;
} // main()


