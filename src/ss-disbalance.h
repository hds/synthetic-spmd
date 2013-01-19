
#ifndef __SS_DISBALANCE_H_
#define	__SS_DISBALANCE_H_

#include "ss-work.h"

typedef struct __SSDisbalanceOp  {
	unsigned int	iteration;
	int		work_unit_delta;
	struct __SSDisbalanceOp	*next;
} SSDisbalanceOp;

// Read Disbalance configuration file
SSDisbalanceOp *readDisbalanceFile(char *filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
SSDisbalanceOp *readDisbalanceLine(char *line, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
int deltaForDisbalanceLine(char **args, unsigned int len, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

// Disbalance operation structure
SSDisbalanceOp *disbalanceOpInit();
void disbalanceOpRelease(SSDisbalanceOp *op);
SSDisbalanceOp *disbalanceOpNext(SSDisbalanceOp *op);

// Apply disbalance
void disbalanceOpApply(SSDisbalanceOp *op, SSWorkArray *work, unsigned int weight_min, unsigned int weight_max);


#endif
