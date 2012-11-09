
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#include "ss-work.h"
#include "ss-communication.h"

#define	WORK_MATRIX_SIZE	(100)

// Define time intervals as longs.
//typedef t_interval	long

typedef struct __SSAppConfig  {
	int		mpi_rank;
	int		mpi_size;
	unsigned int	dims[2];
	unsigned int	wunits;
	unsigned int	wunit_weight[2];
	unsigned int	comm_weight;
	unsigned int	iterations;
	unsigned int	verbose;
} SSAppConfig;

// **** App configuration ****
SSAppConfig *initAppConfig(int argc, char **argv);
SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config);
void releaseAppConfig(SSAppConfig *config);

// **** Application Run Loop ****
void applicationLoop(SSAppConfig *config, SSPeers *peers, SSWorkArray *work_array);
void barrier(SSAppConfig *config);
void work(SSWorkArray *work_array, SSWorkMatrices matrices);
void communication(SSPeers *peers, unsigned int comm_weight, int rank);


#endif
