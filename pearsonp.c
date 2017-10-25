
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
    
    int size  = 100;
    int shift = 5;
    sendcount = size/numtasks;
    
        printf("   size =  %d , sendcount =  %d   ", sendcount, numtasks );
        double *rec_buff = (double *)malloc(sendcount*sizeof(double));
    
        double *sub_avgs = (double *) malloc(sizeof(double)*numtasks );

    
        double *X; 

	X = (double *)malloc(size*sizeof(double));
	double meanx;    
if(rank==0)
    {
        dataInit(X,shift,size);
	
//	 printf("X init array, rank = %d , X = %p \n ", rank, &X   );

	 
	double meanrank0 = mean_func(X,size); 
	printf("this is the is serial mean  %lf \n", meanrank0);  
        MPI_Scatter(X,sendcount,MPI_DOUBLE,rec_buff,sendcount, MPI_DOUBLE,0,MPI_COMM_WORLD);

  
//
        meanx = mean_func(rec_buff,sendcount);
  	MPI_Gather(&meanx,1,MPI_DOUBLE,sub_avgs,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
           
	double finalmean = mean_func(sub_avgs, numtasks);
     
        printf (" parrallel mean %lf ", finalmean);
	}else if (rank != 0)

      {
    
   


        MPI_Scatter(X,sendcount,MPI_DOUBLE,rec_buff,sendcount, MPI_DOUBLE,0,MPI_COMM_WORLD);
        
//	 printf("X init array, rank = %d , X = %p \n ", rank, &X   );

        meanx = mean_func(rec_buff,sendcount);

   

   /* for( int i = 0; i < sendcount; i++)
	{
		printf(" testing scatter rank = %d, i = %d,  rec_buf = %lf \n " ,rank,i,rec_buff[i]);


	}
  // printf (" rank =  %d , meanx = %lf  \n " , rank , meanx  ); 
    
    */

    MPI_Gather(&meanx,1,MPI_DOUBLE,sub_avgs,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
  
    
        
   


    }
   
    /* double *X;
	double *Y;
    X = (double *)malloc(size*sizeof(double));
	Y = (double *)malloc(size*sizeof(double));
	dataInit(X,0,size);	
	dataInit(Y,5,size);
*/


	free(X);
	free(sub_avgs);
	free(rec_buff);
	//double pxy = pxy_func(X,Y,size);

	//printf("This is pxy : %lf \n" , pxy);
MPI_Finalize();
return 0;
}
