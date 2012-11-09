
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#include "ss-work.h"

#define	MAX_PEER_COUNT	(4)
#define SPMD_GRID_DIMS	(2)

#define	GRID_WRAP	(1)
#define	FORCE_GRID_WRAP	(0)

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

typedef struct __SSPeers  {
	unsigned int	n; // number of peers
	int		ids[MAX_PEER_COUNT];
} SSPeers;

typedef enum {
	SSCommDataInitActionNone = 0,
	SSCommDataInitActionZero,
	SSCommDataInitActionRandom
} SSCommDataInitAction;

typedef struct __SSCommData  {
	unsigned int	n; // number of units
	unsigned char	*data;
} SSCommData;

// **** App configuration ****
SSAppConfig *initAppConfig(int argc, char **argv);
SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config);
void releaseAppConfig(SSAppConfig *config);

// **** Communication Data ****
SSCommData *initCommData(unsigned int weight, SSCommDataInitAction init_action);
void releaseCommData(SSCommData *cdata);

// **** Peers (Neighbours) ****
SSPeers initPeers(unsigned int *dims, int rank);

// **** Application Run Loop ****
void applicationLoop(SSAppConfig *config, SSPeers *peers, SSWorkArray *work_array);
void barrier(SSAppConfig *config);
void work(SSWorkArray *work_array, SSWorkMatrices matrices);
void peerCommunication(SSPeers *peers, unsigned int comm_weight, int rank);


#endif
