
#ifndef	__SS_COMMUNICATION_H_
#define __SS_COMMUNICATION_H_

#define	MAX_PEER_COUNT	(4)
#define SPMD_GRID_DIMS	(2)

#define	GRID_WRAP	(1)
#define	FORCE_GRID_WRAP	(0)

// **** Peers (Neighbours) ****
typedef struct __SSPeers  {
	unsigned int	n; // number of peers
	int		ids[MAX_PEER_COUNT];
} SSPeers;

SSPeers *peersInit(unsigned int *dims, int rank);
void peersRelease(SSPeers *);

void peerCommunication(SSPeers *peers, unsigned int comm_weight, int rank);

// **** Communication Data ****
typedef struct __SSCommData  {
	unsigned int	n; // number of units
	unsigned char	*data;
} SSCommData;

typedef enum {
	SSCommDataInitActionNone = 0,
	SSCommDataInitActionZero,
	SSCommDataInitActionRandom
} SSCommDataInitAction;


SSCommData *initCommData(unsigned int weight, SSCommDataInitAction init_action);
void releaseCommData(SSCommData *cdata);


#endif
