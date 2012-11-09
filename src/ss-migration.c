#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "ss-migration.h"
#include "ss-standard.h"

// **** Migration ****

void workUnitMigration(SSPeers *peers, SSWorkArray *work_array, int *movement)
{
	unsigned int 	i, count;
	SSWorkArray	*incoming[MAX_PEER_COUNT];
	SSWorkArray	*outgoing[MAX_PEER_COUNT];
	MPI_Request	req;
	MPI_Status	status;
	int		recv_count;

	for (i = 0; i < MAX_PEER_COUNT; i++)  {
		if (movement[i] > 0)  { // Sending
			count = (unsigned int)movement[i];
			incoming[i] = NULL;
			outgoing[i] = workArrayPopItems(work_array, &count);
			movement[i] = (int)count;
		}
		else if (movement[i] < 0)  { // Receiving
			incoming[i] = workArrayInitWithLength(movement[i] * -1);
			outgoing[i] = NULL;
		}
		else  {
			incoming[i] = NULL;
			outgoing[i] = NULL;
		}
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0 || outgoing[i] == NULL)
			continue;
		MPI_Isend(outgoing[i]->elements, // buf
		          outgoing[i]->length*sizeof(SSWorkUnit), // count
			  MPI_UNSIGNED_CHAR,	// datatype
			  peers->ids[i],	// dest
			  kSSMPITagMigrate,	// tag
		          MPI_COMM_WORLD,	// communicator
			  &req);		// request
		MPI_Request_free(&req);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0 || incoming[i] == NULL)
			continue;
		MPI_Recv(incoming[i]->elements, // buf
		         incoming[i]->length*sizeof(SSWorkUnit), // count (max)
			 MPI_UNSIGNED_CHAR,	// datatype
			 peers->ids[i],		// source
			 kSSMPITagMigrate,	// tag
			 MPI_COMM_WORLD,	// communicator
			 &status);		// status

		MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &recv_count);
		if (recv_count > 0 && recv_count % sizeof(SSWorkUnit) == 0)  {
			incoming[i]->length = recv_count / sizeof(SSWorkUnit);
			workArrayPushItems(work_array, incoming[i]);
		}
	}

	for (i = 0; i < MAX_PEER_COUNT; i++)  {
		workArrayRelease(outgoing[i]);
		workArrayRelease(incoming[i]);
	}

} // workUnitMigration()

