#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#define SIZE 2000000

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
	
	sd = sqrt(sd);	

	return sd;

}

double pxy_func(double *X, double *Y, int size, double meanX, double meanY)
{

	double pxy = 0.0;
	
	for(int i = 0; i <size; i++)
	{
 		
	  pxy += (X[i] -meanX )*(Y[i] - meanY);
		

	}

	

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
	
	sendcount = SIZE/numtasks + 1 ; // partitions for the buffer add 1 to take in to account possible remainder .
    //printf(" HELLO %d \n", sendcount);
	double *X = NULL;
	double *Y = NULL;
	int *sendcounts;
	int *displs;
	sendcounts = (int *)malloc(numtasks*sizeof(int)); //data sent to each processor (remainder will be different)     
	displs = (int *)malloc(numtasks*sizeof(int)); //displacement from send_buff
	double *rec_buffx = (double *)malloc(sendcount*sizeof(double)); //recieve_buff sent to all processes ;
	double *rec_buffy = (double *)malloc(sendcount*sizeof(double));
	int sum =0; //distance from start of sendbuff = X 
	
	 int rem = SIZE % numtasks; //remainder 
	//printf(" ... rem  %d \n" ,rem);
	
	double local_meanX =0.0;
	double global_meanX = 0.0;
	double global_meanY = 0.0;
	double local_meanY =0.0;
	 
 
    X = (double *)malloc(SIZE*sizeof(double));
	Y = (double *)malloc(SIZE*sizeof(double));

	for(int i =0; i<numtasks;i++) // this calculates the remainder cycles around and add remainder to the cores until no rem left . 
	{
		
			sendcounts[i] = sendcount;

		if(rem>0)
		{
			sendcounts[i]++;
			rem--;
		}
	   displs[i] = sum;
	   sum += sendcounts[i];

	   //printf(" int i : %d ,  sendcounts  :  %d ,  displacement %d :  \n ", i ,  sendcounts[i], displs[i]);
	}


	if(rank==0)
	{

     
	 
	
	 
	 dataInit(X,0,SIZE);
	 dataInit(Y,5,SIZE);
	 
	 


	 MPI_Scatterv(X, sendcounts, displs, MPI_DOUBLE, rec_buffx, SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	 MPI_Scatterv(Y, sendcounts, displs, MPI_DOUBLE, rec_buffy, SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	 
	  local_meanX = mean_func( rec_buffx , sendcounts[rank]);
	  local_meanY = mean_func( rec_buffy , sendcounts[rank]);
	 
	  MPI_Allreduce(&local_meanX,&global_meanX,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

	  MPI_Allreduce(&local_meanY,&global_meanY,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

	  printf("this is globalX sum :  %lf , at rank %d ,\n " , global_meanX, rank  );
	  printf("this is globalY sum :  %lf , at rank %d , \n" , global_meanY, rank  );
	  

	}
	else
	{
		MPI_Scatterv(X, sendcounts, displs, MPI_DOUBLE, rec_buffx, SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Scatterv(Y, sendcounts, displs, MPI_DOUBLE, rec_buffy, SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
	   /*if(rank == 2)
		{
			//printf("SENDC2 = %d  \n ",sendcounts[2] );
			for(int i = 0; i < sendcounts[2]; i++)
			{
			//printf(" int i :  %d  ,   rec_buffx  : %.20lf  \n " , i , rec_buffx[i] );
			}
		}*/

		local_meanX = mean_func( rec_buffx , sendcounts[rank]);
		local_meanY = mean_func( rec_buffy , sendcounts[rank]);
	   
		MPI_Allreduce(&local_meanX,&global_meanX,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

		MPI_Allreduce(&local_meanY,&global_meanY,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
		printf("this is globalX sum :  %lf , at rank %d , \n" , global_meanX, rank  );
		printf("this is globalY sum :  %lf , at rank %d , \n " , global_meanY, rank  );
		
	}

	free(rec_buffx);
	free(rec_buffy);
	MPI_Finalize();
}