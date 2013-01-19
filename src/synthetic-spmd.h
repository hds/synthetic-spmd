
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#include "ss-work.h"
#include "ss-communication.h"
#include "ss-timing.h"
#include "ss-disbalance.h"


#define	WORK_MATRIX_SIZE	(100)

#define	SSBadgeBarrier		"Barrier"
#define	SSBadgeWork		"Work"
#define	SSBadgeCommunication	"Communication"
#define	SSBadgeMigration	"Migration"

typedef struct __SSAppConfig  {
	int		mpi_rank;
	int		mpi_size;
	unsigned int	x;
	unsigned int	y;
	unsigned int	dims[2];
	unsigned int	wunits;
	unsigned int	wunit_weight[2];
	unsigned int	comm_weight;
	unsigned int	iterations;
	unsigned int	migration_freq;
	char		*disbalance_file;
	unsigned int	verbose;
} SSAppConfig;

// **** App configuration ****
SSAppConfig *initAppConfig(int argc, char **argv);
SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config);
void releaseAppConfig(SSAppConfig *config);

// **** Application Run Loop ****
void applicationLoop(SSAppConfig *config, SSPeers *peers, SSWorkArray *work_array, SSDisbalanceOp *disbalance_op);
void barrier(int iteration, SSAppConfig *config);
SSDisbalanceOp *disbalance(int iteration, SSDisbalanceOp *op, SSWorkArray *work_array, SSAppConfig *config);
void work(int iteration, int mpi_rank, SSWorkArray *work_array, int work_array_length, SSWorkMatrices matrices);
void communication(int iteration, SSPeers *peers, unsigned int comm_weight, int rank);
void migration(unsigned int iteration, unsigned int migration_freq, SSPeers *peers, SSWorkArray *work_array, int *movement);

// **** Reporting ****
void outputElapsedTime(unsigned int iteration, int mpi_rank, SSTInterval elapsed, char *action);

#endif
