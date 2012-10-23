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
	dims[1] = 2;
	peers = initPeers(dims, mpi_rank);
	    
	MPI_Finalize();

	return 0;
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
