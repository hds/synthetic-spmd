#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ss-migration.h"
#include "ss-standard.h"

// **** Migration ****

void workUnitMigration(SSPeers *peers, SSWorkArray *work_array, int *movement, int iteration)
{
	unsigned int 	i, count;
	SSWorkArray	*incoming[MAX_PEER_COUNT];
	SSWorkArray	*outgoing[MAX_PEER_COUNT];
	MPI_Request	req;
	MPI_Status	status;
	SSWorkUnit	dummy_work_unit;
	SSWorkUnit	*outgoing_work;
	int		recv_count, send_count;
	
	int		mpi_rank;
	char		prefix[40];

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	//sprintf(prefix, "%d\t%d\tPre migration: ", iteration, mpi_rank);
	//workArrayPrintUnitWithPrefix(work_array, prefix);

	for (i = 0; i < MAX_PEER_COUNT; i++)  {
		if (movement[i] > 0)  { // Sending
			count = (unsigned int)movement[i];
			outgoing[i] = workArrayPopItems(work_array, &count);
			movement[i] = (int)count;

			//sprintf(prefix, "%d\t%d\tSend to %d: ", iteration, mpi_rank, peers->ids[i]);
			//workArrayPrintUnitWithPrefix(outgoing[i], prefix);

		}
		else  {
			outgoing[i] = NULL;
		}
		incoming[i] = workArrayInitWithLength(MAX_MIGRATION);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		if (outgoing[i] != NULL)  {
			send_count = outgoing[i]->length * sizeof(SSWorkUnit);
			outgoing_work = outgoing[i]->elements;
		}
		else  {
			send_count = 0;
			outgoing_work = &dummy_work_unit;
		}
		MPI_Isend(outgoing_work,	// buf
		          send_count,		// count
			  MPI_UNSIGNED_CHAR,	// datatype
			  peers->ids[i],	// dest
			  kSSMPITagMigrate,	// tag
		          MPI_COMM_WORLD,	// communicator
			  &req);		// request
		MPI_Request_free(&req);
	}

	for (i = 0; i < peers->n; i++)  {
		if (peers->ids[i] < 0)
			continue;
		MPI_Recv(incoming[i]->elements, // buf
		         incoming[i]->max*sizeof(SSWorkUnit), // count (max)
			 MPI_UNSIGNED_CHAR,	// datatype
			 peers->ids[i],		// source
			 kSSMPITagMigrate,	// tag
			 MPI_COMM_WORLD,	// communicator
			 &status);		// status

		MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &recv_count);
		if (recv_count > 0 && recv_count % sizeof(SSWorkUnit) == 0)  {
			incoming[i]->length = recv_count / sizeof(SSWorkUnit);
			//sprintf(prefix, "%d\t%d\tReceive from %d: ", iteration, mpi_rank, peers->ids[i]);
			//workArrayPrintUnitWithPrefix(incoming[i], prefix);

			workArrayPushItems(work_array, incoming[i]);
		}
	}

	//sprintf(prefix, "%d\t%d\tPost migration: ", iteration, mpi_rank);
	//workArrayPrintUnitWithPrefix(work_array, prefix);

	for (i = 0; i < MAX_PEER_COUNT; i++)  {
		workArrayRelease(outgoing[i]);
		workArrayRelease(incoming[i]);
	}

} // workUnitMigration()

