#include <stdio.h>
#include <math.h>
#include <stdlib.h>
double mean_func(double *data, int size)
{

	double mean = 0.0;
	for(int i = 0; i < size ; size ++)
	{
		
	mean += data[i]; 
	
	}
mean /= size;
return mean;
}

double sd_func(double *data, double l_mean, int size)
{

	double sd = 0.0;
	
	for(int i = 0; i < size ; size ++)
	{
	
	sd += (data[i] - l_mean)*(data[i] - l_mean)
		
	}

	
	sd /= size;

return sd;
}

double pxy_func(double *X, double *Y, double sdy, double sdx)
{

	double pxy = 0.0;

	return pxy;

}

void dataInit( double *data, int shift, int size )
{

	data = malloc(sizeof(double)*size);

	for(int i = 0; i<size; i++)
	{
		data[i] = sin(i+shift);
			
	}



}

void free_data(double *X, double *Y)
{

	free(X);
	free(Y);

}

int main()
{



return 1;
}
