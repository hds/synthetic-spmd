
#ifndef __SS_MIGRATION_H_
#define __SS_MIGRATION_H_

#include "ss-work.h"
#include "ss-communication.h"

//15-1-2013 Andrea
//He cambiado MAX_MIGRATION DE 50 A 100
#define MAX_MIGRATION	(200)

void workUnitMigration(SSPeers *peers, SSWorkArray *work_array, int *movement, int iteration);

#endif
