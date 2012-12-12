
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#include "ss-work.h"
#include "ss-communication.h"
#include "ss-timing.h"

#define	WORK_MATRIX_SIZE	(100)

#define	SSBadgeBarrier		"Barrier"
#define	SSBadgeWork		"Work"
#define	SSBadgeCommunication	"Communication"
#define	SSBadgeMigration	"Migration"

typedef struct __SSAppConfig  {
	int		mpi_rank;
	int		mpi_size;
	unsigned int	dims[2];
	unsigned int	wunits;
	unsigned int	wunit_weight[2];
	unsigned int	comm_weight;
	unsigned int	iterations;
	unsigned int	migration_freq;
	unsigned int	verbose;
} SSAppConfig;

// **** App configuration ****
SSAppConfig *initAppConfig(int argc, char **argv);
SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config);
void releaseAppConfig(SSAppConfig *config);

// **** Application Run Loop ****
void applicationLoop(SSAppConfig *config, SSPeers *peers, SSWorkArray *work_array);
void barrier(int iteration, SSAppConfig *config);
void work(int iteration, int mpi_rank, SSWorkArray *work_array, int work_array_length, SSWorkMatrices matrices);
void communication(int iteration, SSPeers *peers, unsigned int comm_weight, int rank);

// **** Reporting ****
void outputElapsedTime(unsigned int iteration, int mpi_rank, SSTInterval elapsed, char *action);

#endif
