
#ifndef __SS_DISBALANCE_H_
#define	__SS_DISBALANCE_H_

typedef struct __SSDisbalanceOp  {
	unsigned int	iteration;
	int		work_unit_delta;
	struct __SSDisbalanceOp	*next;
} SSDisbalanceOp;

SSDisbalanceOp *readDisbalanceFile(char *filename, unsigned int x, unsigned int y);
SSDisbalanceOp *readDisbalanceLine(char *line, unsigned int x, unsigned int y);

#endif
