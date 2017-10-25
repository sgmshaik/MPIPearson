
#include <stdio.h>
#include <math.h>
#include "mpi.h"

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

	
	sd /= (double)size;
	
	sd = sqrt(sd);	

	return sd;

}

double pxy_func(double *X, double *Y, int size, double meanX, double meanY,double sdx,double sdy)
{

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

	
 
	for(int i = 0; i<size; i++)
	{
		
	data[i] = sin(i+shift);
	
	}



}


int main(void)
{
	

	int numtasks, rank, sendcount, recvcount;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    int size  = 10;
    int shift = 5;
    sendcount = size/(double)numtasks;
    double *X= NULL;
   
    if(rank==0)
    {
       
        X = (double *)malloc(size*sizeof(double));
        dataInit(X,shift,size);
        MPi_Scatter(X,sendcount,MPI_DOUBLE,)


    }
    


   /* double *X;
	double *Y;
    X = (double *)malloc(size*sizeof(double));
	Y = (double *)malloc(size*sizeof(double));
	dataInit(X,0,size);	
	dataInit(Y,5,size);
*/



	//double pxy = pxy_func(X,Y,size);
	//printf("This is pxy : %lf \n" , pxy);


//free(X);
//free(Y);
return 0;
}
