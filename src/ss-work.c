#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ss-work.h"


// **** Work matrices ****

SSWorkMatrices workMatricesInit(int n)
{
	SSWorkMatrices	matrices = {0, NULL, NULL};
	unsigned int	i;

	// We want two matrices.
	if ((matrices.m1 = (double *)malloc(sizeof(double)*n*n)) == NULL)  {
		fprintf(stderr, "Could not assign %dx%d matrix.\n", n, n);
		return matrices;
	}

	if ((matrices.m2 = (double *)malloc(sizeof(double)*n*n)) == NULL)  {
		fprintf(stderr, "Could not assign %dx%d matrix.\n", n, n);
		free(matrices.m1);
		return matrices;
	}

	if ((matrices.r = (double *)malloc(sizeof(double)*n*n)) == NULL)  {
		fprintf(stderr, "Could not assign %dx%d matrix.\n", n, n);
		free(matrices.m1);
		free(matrices.m2);
		return matrices;
	}

	matrices.n = n;

	//srandom(time(NULL));
	for (i = 0; i < matrices.n*matrices.n; i++)  {
		matrices.m1[i] = (double)(random()%SQRT_MAX_EL) * (double)(random()%SQRT_MAX_EL) * (random()%2 ? 1 : -1);
		matrices.m2[i] = (double)(random()%SQRT_MAX_EL) * (double)(random()%SQRT_MAX_EL) * (random()%2 ? 1 : -1);
		matrices.r[i] = 0.0;
	}

	return matrices;
} // initWorkMatrices()

void printSquareMatrix(double *m, unsigned int n, char *name)
{
	unsigned int	i, j;

	for (j = 0; j < n; j++)  {
		if ((n-1)/2 == j)
			printf("%s = |", name);
		else
			printf("     |");
		for (i = 0; i < n; i++)  {
			printf(" %4.0lf", m[j*n + i]);
		}
		printf(" |\n");
	}
	printf("\n");
}

void workMatricesPrint(SSWorkMatrices matrices)
{

	printSquareMatrix(matrices.m1, matrices.n, "m1");
	printSquareMatrix(matrices.m2, matrices.n, "m2");
	printSquareMatrix(matrices.r, matrices.n, "r ");

} // printMatrices()

void workMatricesRelease(SSWorkMatrices *matrices)
{
	if (matrices->m1 != NULL)
		free(matrices->m1);
	matrices->m1 = NULL;

	if (matrices->m2 != NULL)
		free(matrices->m2);
	matrices->m2 = NULL;
} // releaseWorkMatrices()


