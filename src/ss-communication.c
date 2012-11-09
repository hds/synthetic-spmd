#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "ss-communication.h"

// **** Peers (Neighbours) ****

SSPeers *peersInit(unsigned int *dims, int rank)
{
	SSPeers		*peers = NULL;
	unsigned int	x, y;
	unsigned int	i;
	unsigned int	px, py;

	if (SPMD_GRID_DIMS != 2)  {
		fprintf(stderr, "Only 2 dimensional grids currently supported\n");
		return peers;
	}

	if ((peers = (SSPeers *)malloc(sizeof(SSPeers))) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSPeers structure.\n");
		return peers;
	}
	peers->n = 0;

	x = rank % dims[0]; // dims[0] is the grid width
	y = rank / dims[0];

	for (i = 0; i < MAX_PEER_COUNT; i++)
		peers->ids[i] = -1;

	// North peer
	if ((y > 0) || (GRID_WRAP && dims[1] > 2) || (FORCE_GRID_WRAP))  {
		px = x;
		py = (y-1+dims[1]) % dims[1];
		peers->ids[0] = (py*dims[0]) + px;
	}

	// East peer
	if ((x < dims[0]-1) || (GRID_WRAP && dims[0] > 2) || (FORCE_GRID_WRAP))  {
		px = (x+1) % dims[0];
		py = y;
		peers->ids[1] = (py*dims[0]) + px;
	}
	
	// South peer	
	if ((y < dims[1]-1) || (GRID_WRAP && dims[1] > 2) || (FORCE_GRID_WRAP))  {
		px = x;
		py = (y+1) % dims[1];
		peers->ids[2] = (py*dims[0]) + px;
	}

	// West peer
	if ((x > 0) || (GRID_WRAP && dims[0] > 2) || (FORCE_GRID_WRAP))  {
		px = (x-1+dims[0]) % dims[0];
		py = y;
		peers->ids[3] = (py*dims[0]) + px;
	}

	peers->n = 4;
//	printf("[%d] I have coords %d,%d\n", rank, x, y);
//	printf("    [%d] neigh: %d, %d, %d, %d\n", rank, peers->ids[0], peers->ids[1], peers->ids[2], peers->ids[3]);

	return peers;
} // peersInit()

void peersRelease(SSPeers *peers)
{
	if (peers != NULL)
		free(peers);
	peers = NULL;
} // peersRelease()

void peerCommunication(SSPeers *peers, unsigned int comm_weight, int rank)
{
	unsigned int 	i;
	SSCommData	*send, *recv;
	MPI_Request	req;
	MPI_Status	status;

	// Sanity check
	if (comm_weight == 0)
		return;

	send = initCommData(comm_weight, SSCommDataInitActionZero);
	recv = initCommData(comm_weight, SSCommDataInitActionNone);

	if (send == NULL || recv == NULL)  {
		fprintf(stderr, "Sending or receiving data couldn't be allocated.\n");
		return;
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		MPI_Isend(send->data,		// buf
		          send->n,		// count
			  MPI_UNSIGNED_CHAR,	// datatype
			  peers->ids[i],	// dest
			  123,			// tag
		          MPI_COMM_WORLD,	// communicator
			  &req);		// request
		MPI_Request_free(&req);
		//printf("[%d] Sent %d to neighbour %d (%d)\n", rank, local_data, i, peers->ids[i]);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		//printf("[%d] ready to receive from neighbour %d (%d)\n", rank, i, peers->ids[i]);
		MPI_Recv(recv->data,		// buf
		         recv->n,		// count (max)
			 MPI_UNSIGNED_CHAR,	// datatype
			 peers->ids[i],		// source
			 123,			// tag
			 MPI_COMM_WORLD,	// communicator
			 &status);		// status
		//printf("[%d] Received %d from neighbour %d (%d)\n", rank, recv_data, i, peers->ids[i]);
	}

	releaseCommData(send);
	releaseCommData(recv);


} // peerCommunication()


// **** Communication Data ****

SSCommData *initCommData(unsigned int weight, SSCommDataInitAction init_action)
{
	SSCommData	*cdata = NULL;
	unsigned int	i;

	if ((cdata = (SSCommData*)malloc(sizeof(SSCommData))) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSCommData structure.\n");
		return cdata;
	}

	cdata->n = weight;
	cdata->data = NULL;
	if ((cdata->data = (unsigned char *)malloc(sizeof(unsigned char)*cdata->n)) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSCommData data.\n");
		free(cdata);
		cdata = NULL;
		return cdata;
	}

	if (init_action == SSCommDataInitActionNone)  {
		// Nothing.
	}
	else if (init_action == SSCommDataInitActionZero)  {
		for (i = 0; i < cdata->n; i++)  {
			cdata->data[i] = (unsigned char)(0);
		}
	}
	else if (init_action == SSCommDataInitActionRandom)  {
		for (i = 0; i < cdata->n; i++)  {
			cdata->data[i] = (unsigned char)(random()%sizeof(unsigned char));
		}
	}

	return cdata;
} // initCommData()

void releaseCommData(SSCommData *cdata)
{
	if (cdata->data != NULL)
		free(cdata->data);
	cdata->data = NULL;

	if (cdata != NULL)
		free(cdata);
} // releaseCommData()

