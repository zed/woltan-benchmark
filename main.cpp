#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>
#include <time.h>
#include <sys/time.h>

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

using namespace std;

#define REPEAT 20
#ifndef LIBBLAS_SO
#define LIBBLAS_SO libblas.so
#endif /* !LIBBLAS_SO */

typedef void (*func_sgemm)(char*, char*, int*, int*, int*, float*, float*, int*, float*, int*, float*, float*, int*);

void* GetLibrarayFunction(string function)
{
  void* handle = NULL;

  handle = dlopen(STR(LIBBLAS_SO), RTLD_LAZY);

  if(!handle)
    throw "Could not load library";

  void* Func = dlsym(handle, function.c_str());

  char* result = dlerror();

  if(result)
     throw result;

  return Func;
}

double CalcTime(timeval start, timeval end)
{
  double factor = 1000000;
  return (((double)end.tv_sec) * factor + ((double)end.tv_usec) - (((double)start.tv_sec) * factor + ((double)start.tv_usec))) / factor;
}

double min(double* dTimes)
{
	double min = dTimes[0];
	for(int i = 1; i < REPEAT; i++)
	{
		if(dTimes[i] < min)
			min = dTimes[i];
	}
	return min;
}

double max(double* dTimes)
{
	double max = dTimes[0];
	for(int i = 1; i < REPEAT; i++)
	{
		if(dTimes[i] > max)
			max = dTimes[i];
	}
	return max;
}

double mean(double* dTimes)
{
	double sum = 0.0;
	for(int i = 0; i < REPEAT; i++)
	{
		sum += dTimes[0];
	}
	return sum/REPEAT;
}

int main(int argc, char *argv[])
{
	func_sgemm f = (func_sgemm)GetLibrarayFunction("sgemm_");
	char no_trans('n');
	float zero(0);
	float one(1.0);

	timeval start, end;

	FILE *pF = fopen("cpp.csv", "w");
	FILE *pFr = fopen("cppraw.csv", "w");

	double dTimes[REPEAT];
	for(int i = 0; i < argc - 1; ++i)
	{
		int dim = atoi(argv[i + 1]);
		float A[dim * dim];
		float B[dim * dim];

		for(int j = 0; j < dim; j++)
		{
			for(int k = 0; k < dim; k++)
			{
				A[j * dim + k] = j*dim + k;
				if(j == k)
					B[j*dim + k] = 1.0;
				else
					B[j*dim + k] = 0.0;
			}
		}
		float Return[dim * dim];

		for(int j = 0; j < REPEAT; j++)
		{
			gettimeofday(&start, NULL);
			f(&no_trans, &no_trans, &dim, &dim, &dim, &one, A, &dim, B, &dim, &zero, Return, &dim);
			gettimeofday(&end, NULL);
			dTimes[j] = CalcTime(start, end);
			fprintf(pFr, "%lf ", dTimes[j]);
		}
		fprintf(pF, "%d %lf %lf %lf\n", dim, mean(dTimes), min(dTimes), max(dTimes));
		fprintf(pFr, "\n");

		for(int j = 0; j < dim; j++)
		{
			for(int k = 0; k < dim; k++)
			{
				if(A[j*dim + k] != Return[j*dim + k])
					printf("%lf != %lf\n", A[j*dim + k], Return[j*dim + k]);
			}
		}
	}

	fclose(pF);
	return 0;
}
