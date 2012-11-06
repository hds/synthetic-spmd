#include <time.h>
#include <sys/time.h>

#include "ss-timing.h"

SSTInterval getCurrentTime()
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	return ((SSTInterval)tv.tv_sec * 1000000) + (SSTInterval)(tv.tv_usec);
} // SSTGetCurrentTime()

SSTInterval elapsedTime(SSTInterval b)
{
	return getCurrentTime() - b;
} // SSTElapsedTime()

