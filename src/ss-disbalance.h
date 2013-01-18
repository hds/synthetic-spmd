
#ifndef __SS_DISBALANCE_H_
#define	__SS_DISBALANCE_H_

typedef struct __SSDisbalanceOp  {
	unsigned int	iteration;
	int		work_unit_delta;
	struct __SSDisbalanceOp	*next;
} SSDisbalanceOp;

SSDisbalanceOp *readDisbalanceFile(char *filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
SSDisbalanceOp *readDisbalanceLine(char *line, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
int deltaForDisbalanceLine(char **args, unsigned int len, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

SSDisbalanceOp *disbalanceOpNew();
void disbalanceOpRelease(SSDisbalanceOp *op);


#endif
