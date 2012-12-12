
#ifndef __SS_MIGRATION_H_
#define __SS_MIGRATION_H_

#include "ss-work.h"
#include "ss-communication.h"

#define MAX_MIGRATION	(50)

void workUnitMigration(SSPeers *peers, SSWorkArray *work_array, int *movement, int iteration);

#endif
