
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#define	MAX_PEER_COUNT	(4)
#define SPMD_GRID_DIMS	(2)

#define	GRID_WRAP	(1)
#define	FORCE_GRID_WRAP	(0)


// Define time intervals as longs.
//typedef t_interval	long

struct __SSAppConfig  {
	unsigned int	dims[2];
	unsigned int	wunits;
	unsigned int	wunit_weight[2];
	unsigned int	comm_weight;
	unsigned int	verbose;
};
typedef struct __SSAppConfig SSAppConfig;

struct __SSPeers  {
	unsigned int	n; // number of peers
	int		ids[MAX_PEER_COUNT];
};
typedef struct __SSPeers SSPeers;
typedef struct __SSPeers * SSPeersRef;

struct __SSWorkUnit  {
	unsigned int	weight; // weight
};
typedef struct __SSWorkUnit SSWorkUnit;

SSAppConfig *initAppConfig(int argc, char **argv, int mpi_size);
void releaseAppConfig(SSAppConfig *config);

SSPeers initPeers(unsigned int *dims, int rank);

void work(SSWorkUnit *work_units);
void peerCommunication(SSPeers *peers, int rank);

//SSPeersRef ssPeersCreate();
//void ssPeersRelease(SSPeersRef peers);

#endif
