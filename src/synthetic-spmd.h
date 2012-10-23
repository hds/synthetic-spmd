
#ifndef __SYNTHETIC_SPMD_H_
#define __SYNTHETIC_SPMD_H_

#define	MAX_PEER_COUNT	(4)
#define SPMD_GRID_DIMS	(2)

#define	GRID_WRAP	(1)
#define	FORCE_GRID_WRAP	(0)


// Define time intervals as longs.
//typedef t_interval	long

struct __SSPeers  {
	unsigned int	n; // number of peers
	int		ids[MAX_PEER_COUNT];
};
typedef struct __SSPeers SSPeers;
typedef struct __SSPeers * SSPeersRef;

SSPeers initPeers(unsigned int *dims, int rank);


//SSPeersRef ssPeersCreate();
//void ssPeersRelease(SSPeersRef peers);

#endif
