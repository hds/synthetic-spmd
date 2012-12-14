#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ss-work.h"
#include "ss-timing.h"

unsigned int maxForArrayLength(unsigned int length)
{
	return (length/WORK_BLOCK_SIZE) * WORK_BLOCK_SIZE + (length % WORK_BLOCK_SIZE ? WORK_BLOCK_SIZE : 0);
} // maxForArrayLength()

SSWorkArray *workArrayInitWithLength(unsigned int length)
{
	SSWorkArray	*array = NULL;

	if ((array = (SSWorkArray *)malloc(sizeof(SSWorkArray))) == NULL)  {
		fprintf(stderr, "Could not allocate memory for SSWorkArray structure.\n");
		return array;
	}

	array->length = length;
	array->max = maxForArrayLength(array->length);
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
	// Safety.
	if (array == NULL)
		return;

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

	if (min != max)  {
		for (i = 0; i < array->length; i++)  {
			array->elements[i].weight = random()%(max-min+1) + min;
		}
	}
	else  {
		for (i = 0; i < array->length; i++)  {
			array->elements[i].weight = max;
		}
	}
} // workArrayFillUnits()

void workArrayPrintUnitWithPrefix(SSWorkArray *array, char *prefix)
{
	unsigned int	i;
	char		array_print[200] = "\0";

	strcpy(array_print, "[ ");
	for (i = 0; i < array->length; i++)  {
		
		sprintf(&(array_print[strlen(array_print)]),"%s%d", i ? ", " : "", array->elements[i].weight);
	}
	strcat(array_print, "]");

	printf("%s%s\n", prefix, array_print);
} // workArrayPrintUnitWithPrefix()

void workArrayPrintUnits(SSWorkArray *array)
{
	unsigned int	i;

	printf("[ ");
	for (i = 0; i < array->length; i++)  {
		printf("%s%d", i ? ", " : "", array->elements[i].weight);
	}
	printf(" ]\n");
} // workArayPrintUnits()

void workArrayPushItems(SSWorkArray *array, SSWorkArray *new_items)
{
	unsigned int	i, offset;

	offset = array->length;
	if (offset + new_items->length > array->max)  {
		// Enlarge array.
		unsigned int	new_max;
		void		*new_elements;
		new_max = maxForArrayLength(offset + new_items->length);

		if ((new_elements = realloc(array->elements, new_max*sizeof(SSWorkArray))) == NULL)  {
			fprintf(stderr, "Could not enlarge SSWorkArray elements space. This is BAD.\n");
			return;
		}

		array->elements = (SSWorkUnit *)new_elements;
		array->max = new_max;
	}


	for (i = 0; i < new_items->length; i++)  {
		array->elements[offset+i] = new_items->elements[i];
	}
	array->length += new_items->length;
	
} // workArrayPushItems()

SSWorkArray *workArrayPopItems(SSWorkArray *array, unsigned int *count)
{
	SSWorkArray	*poppedItems;
	unsigned int	i, offset;

	if (*count > array->length)
		*count = array->length;

	poppedItems = workArrayInitWithLength(*count);

	offset = array->length - *count;
	for (i = 0; i < *count; i++)  {
		poppedItems->elements[i] = array->elements[i+offset];
	}
	array->length = array->length - *count;	

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
} // workMatricesRelease()

// **** Time based work ****

int workBusy(long double milisec)
{
	SSTInterval	t1, t2;
	int		counter = 0;
	double		k = 3.1415;

	t1 = getCurrentTime();
	do  {
		++counter;
		k = k * counter + counter / 2.7;
		t2 = getCurrentTime();
	} while ((t2-t1)<(milisec*1000));

	return 0;
}

