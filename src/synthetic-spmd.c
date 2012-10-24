#include <stdio.h>
#include <mpi.h>

#include "synthetic-spmd.h"

int main(int argc, char **argv)
{
	int mpi_rank, mpi_size;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

	//printf("Hello World from Node %d of %d\n", mpi_rank, mpi_size);

	SSPeers	peers;
	unsigned int	dims[SPMD_GRID_DIMS];
	dims[0] = 2;
	dims[1] = 1;
	peers = initPeers(dims, mpi_rank);

	peerCommunication(&peers, mpi_rank);
	    
	MPI_Finalize();

	return 0;
}

void work(SSWorkUnit *work_units)
{
	// Do work
}

void peerCommunication(SSPeers *peers, int rank)
{
	unsigned int 	i;
	int		local_data, recv_data;
	MPI_Request	req;
	MPI_Status	status;
	local_data = rank;

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		MPI_Isend(&local_data,		// buf
		          1,			// count
			  MPI_INT,		// datatype
			  peers->ids[i],	// dest
			  123,			// tag
		          MPI_COMM_WORLD,	// communicator
			  &req);		// request
		MPI_Request_free(&req);
		printf("[%d] Sent %d to neighbour %d (%d)\n", rank, local_data, i, peers->ids[i]);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		printf("[%d] ready to receive from neighbour %d (%d)\n", rank, i, peers->ids[i]);
		MPI_Recv(&recv_data,		// buf
		         1,			// count (max)
			 MPI_INT,		// datatype
			 peers->ids[i],		// source
			 123,			// tag
			 MPI_COMM_WORLD,	// communicator
			 &status);		// status
		printf("[%d] Received %d from neighbour %d (%d)\n", rank, recv_data, i, peers->ids[i]);
	}


}

SSPeers initPeers(unsigned int *dims, int rank)
{
	SSPeers		peers;
	peers.n = 0;

	if (SPMD_GRID_DIMS != 2)  {
		fprintf(stderr, "Only 2 dimensional grids currently supported\n");
		return peers;
	}
	unsigned int	x, y;
	unsigned int	i;

	x = rank % dims[0]; // dims[0] is the grid width
	y = rank / dims[0];

	for (i = 0; i < MAX_PEER_COUNT; i++)
		peers.ids[i] = -1;

	// North peer
	if ((y > 0) || (GRID_WRAP && dims[1] > 2) || (FORCE_GRID_WRAP))  {
		peers.ids[0] = ((y-1)%dims[1])*dims[0] + x;
	}

	// East peer
	if ((x < dims[0]-1) || (GRID_WRAP && dims[0] > 2) || (FORCE_GRID_WRAP))  {
		peers.ids[1] = (y*dims[0]) + ((x+1)%dims[0]);
	}
	
	// South peer	
	if ((y < dims[1]-1) || (GRID_WRAP && dims[1] > 2) || (FORCE_GRID_WRAP))  {
		peers.ids[2] = ((y-1)%dims[1])*dims[0] + x;
		//peers.ids[2] = (rank + dims[0]) % (dims[0]*dims[1]);
	}

	// West peer
	if ((x > 0) || (GRID_WRAP && dims[0] > 2) || (FORCE_GRID_WRAP))  {
		peers.ids[3] = (y*dims[0]) + ((x-1)%dims[0]);
	}

	peers.n = 4;
	printf("[%d] I have coords %d,%d\n", rank, x, y);
	printf("    [%d] neigh: %d, %d, %d, %d\n", rank, peers.ids[0], peers.ids[1], peers.ids[2], peers.ids[3]);

	return peers;
}

//SSPeersRef ssPeersCreate()
//{
//	SSPeersRef	peers = NULL;
//
//	if ((peers = malloc(sizeof(struct __SSPeers))) == NULL)  {
//		fprintf(stderr, "Could not create instance of SSPeersRef, out of memory\n");
//		return peers;
//	}
//
//	return peers;
//}
//
//void ssPeersRelease(SSPeersRef peers)
//{
//	if (peers != NULL)
//		free(peers);
//}
