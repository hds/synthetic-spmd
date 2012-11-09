
#ifndef __SS_WORK_H_
#define __SS_WORK_H_

// Largest size element in the work array.
#define	SQRT_MAX_EL	(4)



// **** Units of Work ****
typedef struct __SSWorkUnit  {
	unsigned int	weight; // weight
} SSWorkUnit;

typedef struct __SSWorkArray  {
	unsigned int	length;
	unsigned int	max;
	SSWorkUnit	*elements;
} SSWorkArray;

SSWorkArray *workArrayInitWithLength(unsigned int length);
void workArrayRelease(SSWorkArray *array);

void workArrayFillUnits(SSWorkArray *array, unsigned int min, unsigned int max);
void workArrayPrintUnits(SSWorkArray *array);
void workArrayPushItems(SSWorkArray *array, SSWorkUnit *items, unsigned int count);
SSWorkUnit *workArrayPopItems(SSWorkArray *array, unsigned int *count);

// **** Work matrices ****
typedef struct __SSWorkMatrices  {
	unsigned int	n;
	double		*m1;
	double		*m2;
	double		*r;
} SSWorkMatrices;

SSWorkMatrices workMatricesInit(int n);
void workMatricesMultiply(SSWorkMatrices matrices);
void workMatricesPrint(SSWorkMatrices matrices);
void workMatricesRelease(SSWorkMatrices *matrices);



#endif

