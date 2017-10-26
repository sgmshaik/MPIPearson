#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
double mean_func(double *data, int size)
{

	double mean = 0.0;
	for(int i = 0; i < size ; i++)
	{
		
	mean += data[i]; 
	
	}
	
	mean /= (double)size;

	
	return mean;
}

double sd_func(double *data, double l_mean, int size)
{

	double sd = 0.0;
	
	for(int i = 0; i < size ; i ++)
	{
	
	sd += (data[i] - l_mean)*(data[i] - l_mean);

		
	}

	
	//sd /= (double)size;
	
	sd = sqrt(sd);	

	return sd;

}

double pxy_func(double *X, double *Y, int size)
{

	double meanX = mean_func(X,size);
 	double meanY = mean_func(Y, size);
	
	double sdx = sd_func(X,meanX,size);
	double sdy = sd_func(Y,meanY,size);
	
	//printf("This is the mean : %.20lf \n" , meanY);
	double pxy = 0.0;
	
	for(int i = 0; i <size; i++)
	{
 		
	  pxy += (X[i] -meanX )*(Y[i] - meanY);
		

	}

	pxy /= (sdx*sdy);
	
	return pxy;

}

void dataInit( double *data, int shift, int size )
{

	
 
	for(int i = 0; i<size; i++)
	{
		
	data[i] = sin(i+shift);
	
	}



}


int main()
{
	

	double *X;
	double *Y;
	int size  = 2.4*pow(10,8);
    int shift = 5;
	
	X = (double *)malloc(size*sizeof(double));
	Y = (double *)malloc(size*sizeof(double));
	
	dataInit(X,0,size);	
	dataInit(Y,5,size);


	clock_t start, finish;
	
	start = clock();

	double pxy = pxy_func(X,Y,size);
	finish = clock();	
	double elapsedt = (double)(finish - start)/ CLOCKS_PER_SEC; 
	printf("This is pxy : %lf and the time is : %lf  \n" , pxy, elapsedt);


free(X);
free(Y);
return 1;
}
