#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ss-work.h"

#define	WORK_BLOCK_SIZE	(100)

SSWorkArray *workArrayInitWithLength(unsigned int length)
{
	SSWorkArray	*array = NULL;

	if ((array = (SSWorkArray *)malloc(sizeof(SSWorkArray))) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSWorkArray structure.\n");
		return array;
	}

	array->length = length;
	array->max = (length/WORK_BLOCK_SIZE) * WORK_BLOCK_SIZE + (length % WORK_BLOCK_SIZE ? WORK_BLOCK_SIZE : 0);
	array->elements = NULL;
	if ((array->elements = (SSWorkUnit *)malloc(sizeof(SSWorkUnit)*array->max)) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSWorkArray elements.\n");
		free(array);
		array = NULL;
		return array;
	}

	return array;
} // workArrayInitWithLength()

void workArrayRelease(SSWorkArray *array)
{
	if (array->elements != NULL)
		free(array->elements);
	array->elements = NULL;	

	if (array != NULL)
		free(array);
	array = NULL;
} // workArrayRelease()

void workArrayFillUnits(SSWorkArray *array, unsigned int min, unsigned int max)
{
	unsigned int	i;

	for (i = 0; i < array->length; i++)  {
		array->elements[i].weight = random()%(max-min+1) + min;
	}
} // workArrayFillUnits()

void workArrayPrintUnits(SSWorkArray *array)
{
	unsigned int	i;

	printf("[ ");
	for (i = 0; i < array->length; i++)  {
		printf("%s%d", i ? ", " : "", array->elements[i].weight);
	}
	printf(" ]\n");
} // workArayPrintUnits()

void workArrayPushItems(SSWorkArray *array, SSWorkUnit *items, unsigned int count)
{

} // workArrayPushItems()

SSWorkUnit *workArrayPopItems(SSWorkArray *array, unsigned int *count)
{
	SSWorkUnit	*poppedItems = NULL;

	return poppedItems;
} // workArrayPushItems()


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

void workMatricesMultiply(SSWorkMatrices matrices)
{
	unsigned int	i, j, k;

	// Multiply matrices r <-- m1 x m2
	for (j = 0; j < matrices.n; j++)  {
		for (i = 0; i < matrices.n; i++)  {
			// j-th row of m1 x i-th column of m2
			matrices.r[j*matrices.n + i] = 0;
			for (k = 0; k < matrices.n; k++)  {
				matrices.r[j*matrices.n + i] += matrices.m1[j*matrices.n + k] * matrices.m2[k*matrices.n + i];
			}
		}
	}
} // workMatricesMultiply()

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


