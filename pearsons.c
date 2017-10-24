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
	
	mean /= (double)size;

	return mean;
}

double sd_func(double *data, double l_mean, int size)
{

	double sd = 0.0;
	
	for(int i = 0; i < size ; size ++)
	{
	
	sd += (data[i] - l_mean)*(data[i] - l_mean);

		
	}

	
	sd /= (double)size;
	
	sd = sqrt(sd);	

	return sd;

}

double pxy_func(double *X, double *Y, int size)
{

	double meanX = mean_func(X,size);
 	double meanY = mean_func(Y, size);
	double sdx = sd_func(X,meanX,size);
	double sdy = sd_func(Y,meanY,size);
		


	double pxy = 0.0;
	
	for(int i = 0; i <size; i++)
	{
 		
	  pxy += (X[i] -meanX )*(Y[i] - meanY);
		

	}

	pxy /= (double)size*(sdx*sdy);
	
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


//free_data(X,Y);

return 1;
}
