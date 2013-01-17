#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ss-disbalance.h"

#define	LINE_BUFFER_SIZE	128

SSDisbalanceOp *readDisbalanceFile(char *filename, unsigned int x, unsigned int y)
{
	SSDisbalanceOp	*first = NULL, *last = NULL, *current;
	FILE			*fh;
	char			line_buffer[LINE_BUFFER_SIZE];

	fh = fopen(filename, "r");
	if (fh == NULL)  {
		fprintf(stderr, "Error opening disbalance file: %s\n", filename);
		return NULL;
	}

	while (fgets(line_buffer, LINE_BUFFER_SIZE-1, fh) != NULL)  {
		while (line_buffer[strlen(line_buffer)-1] == '\n')
			line_buffer[strlen(line_buffer)-1] = '\0';

		fprintf(stdout, "%u x %u: [%s]\n", x, y, line_buffer);

		current = readDisbalanceLine(line_buffer, x, y);

		if (last == NULL)  {
			first = current;
			last = current;
		}
		else  {
			last->next = current;
		}
	}

	return first;
} // readDisbalanceFile()

SSDisbalanceOp *readDisbalanceLine(char *line, unsigned int x, unsigned int y)
{
	SSDisbalanceOp	*op = NULL;

	if ((op = (SSDisbalanceOp *)malloc(sizeof(SSDisbalanceOp))) == NULL)  {
		fprintf(stderr, "Error allocating memory for disbalance operation line: %s\n", line);
		return NULL;
	}
	
	// FIXME: This is obviously wrong.
	op->iteration = 3;
	op->work_unit_delta = 20;

	return op;
} // readDisbalanceLine()
