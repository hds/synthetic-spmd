#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ss-disbalance.h"

#define	LINE_BUFFER_SIZE	128
#define	MAX_ARGS		5

SSDisbalanceOp *readDisbalanceFile(char *filename, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	SSDisbalanceOp	*first = NULL, *last = NULL, *current;
	FILE			*fh;
	char			line_buffer[LINE_BUFFER_SIZE], line_copy[LINE_BUFFER_SIZE];

	fh = fopen(filename, "r");
	if (fh == NULL)  {
		fprintf(stderr, "Error opening disbalance file: %s\n", filename);
		return NULL;
	}

	while (fgets(line_buffer, LINE_BUFFER_SIZE-1, fh) != NULL)  {
		while (line_buffer[strlen(line_buffer)-1] == '\n' || line_buffer[strlen(line_buffer)-1] == '\r')
			line_buffer[strlen(line_buffer)-1] = '\0';
		strcpy(line_copy, line_buffer);

		// Get the disbalance operation for this line
		current = readDisbalanceLine(line_copy, x, y, width, height);

		if (current == NULL)
			continue;

		if (last == NULL)  {
			first = current;
			last = current;
		}
		else  {
			fprintf(stderr, "%d -> %d\n", last->iteration, current->iteration);
			if (last->iteration <= current->iteration)  {
				last->next = current;
				last = current;
			}
			else  {
				fprintf(stderr, "Error, disbalance lines must be ordered by iteration, dropping line: %s\n", line_buffer);
				disbalanceOpRelease(current);
			}
		}
	}

	return first;
} // readDisbalanceFile()

SSDisbalanceOp *readDisbalanceLine(char *line, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	SSDisbalanceOp	*op = NULL;
	char		*args[MAX_ARGS];
	unsigned int	i, s, len;
	int		delta = 0;

	len = strlen(line);
	args[0] = &line[0];
	for (s = 1, i = 1; s < len; s++)  {
		if (line[s] == ' ' || line[s] == '\t')  {
			line[s] = '\0';
		}
		else if (line[s-1] == '\0' && line[s] != ' ' && line[s] != '\t')  {
			if (i >= MAX_ARGS)
				break;

			printf("%d: args[%d] = &line[%d]: %s\n", y*width+x, i, s, &line[s]);
			args[i] = &line[s];
			i++;
		}
	}
	len = i;

	if (len < 3)  {
		fprintf(stderr, "Error, bad disbalance config line: [%s]\n", line);
		return NULL;
	}

	// Get this node's delta for this disbalance
	delta = deltaForDisbalanceLine(args, len, x, y, width, height);

	if (delta > 0)  {
		if ((op = disbalanceOpNew()) == NULL)  {
			fprintf(stderr, "Error allocating memory for disbalance operation line: %s\n", line);
			return NULL;
		}

		op->iteration = atoi(args[0]);
		op->work_unit_delta = delta;
	}

	return op;
} // readDisbalanceLine()

SSDisbalanceOp *disbalanceOpNew()
{
	SSDisbalanceOp	*op;

	if ((op = (SSDisbalanceOp *)malloc(sizeof(SSDisbalanceOp))) == NULL)  {
		return NULL;
	}

	op->iteration = 0;
	op->work_unit_delta = 0;
	op->next = NULL;

	return op;
} // disbalanceOpNew()

void disbalanceOpRelease(SSDisbalanceOp *op)
{
	if (op != NULL)
		free(op);
} // disbalanceOpRelease()

int deltaForEdgeLine(char **args, unsigned int len, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	int	depth, delta = 0;

	if (len < 5)  {
		fprintf(stderr, "Error, an edge disbalance requires 5 fields.\n");
		return 0;
	}

	depth = atoi(args[3]);
	switch (args[2][0])  {
		case 'N':
			if (y < depth)
				delta = atoi(args[4]);
			break;
		case 'E':
			if (x >= width-depth)
				delta = atoi(args[4]);
			break;
		case 'S':
			if (y >= height-depth)
				delta = atoi(args[4]);
			break;
		case 'W':
			if (x < depth)
				delta = atoi(args[4]);
			break;
	}

	return delta;
} // deltaForEdgeLine()

int deltaForAlledgesLine(char **args, unsigned int len, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	int	depth, delta = 0;

	if (len < 4)  {
		fprintf(stderr, "Error, an alledge disbalance requires 4 fields.\n");
		return 0;
	}

	depth = atoi(args[2]);
	if (y < depth || x >= width-depth || y >= height-depth || x < depth)  {
		delta = atoi(args[3]);
	}

	return delta;
} // deltaForAlledgesLine()

int deltaForRowLine(char **args, unsigned int len, unsigned int x, unsigned int y)
{
	int	delta = 0;

	if (len < 4)  {
		fprintf(stderr, "Error, a row disbalance requires 4 fields.\n");
		return 0;
	}

	if (atoi(args[2]) == y)
		delta = atoi(args[3]);

	return delta;
} // deltaForRowLine()

int deltaForColumnLine(char **args, unsigned int len, unsigned int x, unsigned int y)
{
	int	delta = 0;

	if (len < 4)  {
		fprintf(stderr, "Error, a column disbalance requires 4 fields.\n");
		return 0;
	}

	if (atoi(args[2]) == x)
		delta = atoi(args[3]);

	return delta;
} // deltaForColumnLine()

int deltaForHotspotLine(char **args, unsigned int len, unsigned int x, unsigned int y)
{
	int	delta = 0;
	int	radius, hx, hy;

	if (len < 5)  {
		fprintf(stderr, "Error, a hotspot disbalance requires 5 fields.\n");
		return 0;
	}

	if (sscanf(args[2], "%dx%d", &hx, &hy) != 2)  {
		fprintf(stderr, "Error, bad hotspot disbalance coordinates: %s\n", args[2]);
		return 0;
	}
	radius = atoi(args[3]);

	if (abs(hx-(int)x) + abs(hy-(int)y) < radius)
		delta = atoi(args[4]);

	return delta;
} // deltaForHotspotLine()

int deltaForGlobalLine(char **args, unsigned int len)
{
	return atoi(args[2]);
} // deltaForGlobalLine()

int deltaForDisbalanceLine(char **args, unsigned int len, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	int	delta = 0;

	if (strcmp(args[1], "edge") == 0)  {
		delta = deltaForEdgeLine(args, len, x, y, width, height);
	}
	else if (strcmp(args[1], "alledges") == 0)  {
		delta = deltaForAlledgesLine(args, len, x, y, width, height);
	}
	else if (strcmp(args[1], "row") == 0)  {
		delta = deltaForRowLine(args, len, x, y);
	}
	else if (strcmp(args[1], "column") == 0)  {
		delta = deltaForColumnLine(args, len, x, y);
	}
	else if (strcmp(args[1], "hotspot") == 0)  {
		delta = deltaForHotspotLine(args, len, x, y);
	}
	else if (strcmp(args[1], "global") == 0)  {
		delta = deltaForGlobalLine(args, len);
	}
	else  {
		fprintf(stderr, "Error, unknown disbalance type: %s\n", args[1]);
	}

	return delta;
} // deltaForDisbalanceLine()
