
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#define	MAX_PEER_COUNT	(4)
#define SPMD_GRID_DIMS	(2)

#define	GRID_WRAP	(1)
#define	FORCE_GRID_WRAP	(0)

#define	WORK_MATRIX_SIZE	(100)

#define	SQRT_MAX_EL	(4)

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

typedef struct __SSWorkUnit  {
	unsigned int	weight; // weight
} SSWorkUnit;

typedef struct __SSWorkMatrices  {
	unsigned int	n;
	double		*m1;
	double		*m2;
	double		*r;
} SSWorkMatrices;

// **** App configuration ****
SSAppConfig *initAppConfig(int argc, char **argv);
SSAppConfig *displayUsageAndReleaseConfig(SSAppConfig *config);
void releaseAppConfig(SSAppConfig *config);

// **** Communication Data ****
SSCommData *initCommData(unsigned int weight, SSCommDataInitAction init_action);
void releaseCommData(SSCommData *cdata);

// **** Peers (Neighbours) ****
SSPeers initPeers(unsigned int *dims, int rank);

// **** Work matrices ****
SSWorkMatrices initWorkMatrices(int n);
void printWorkMatrices(SSWorkMatrices matrices);
void releaseWorkMatrices(SSWorkMatrices *matrices);

// **** Application Run Loop ****
void applicationLoop(SSAppConfig *config, SSPeers peers, SSWorkMatrices matrices);
void work(SSWorkUnit *work_units, SSWorkMatrices matrices);
void peerCommunication(SSPeers *peers, int rank);


#endif
