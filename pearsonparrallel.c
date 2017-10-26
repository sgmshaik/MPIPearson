#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#define SIZE 240000000



 double mean_func(double *data, int size)
{

	double mean = 0.0;
	for(int i = 0; i < size ; i++)
	{
		
	mean += data[i]; 
	
	}
	
	mean /= (double)SIZE;

	
	return mean;
}

 double sd_func(double *data, double l_mean, int size)
{

	double sd = 0.0;
	
	for(int i = 0; i < size ; i ++)
	{
	
	sd += (data[i] - l_mean)*(data[i] - l_mean);

		
	}
	
	

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

int main()
{
	

    int numtasks, rank, sendcount_int, recvcount;
    MPI_Init(NULL, NULL);
    /*if(argc == 1)
	{
		
	} */  
    

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	
	sendcount_int = SIZE/numtasks ; // partitions for the buffer add 1 to take in to account possible remainder .
    //printf(" HELLO %d \n", sendcount);
        double *X = NULL;
	double *Y = NULL;
	int *sendcounts;
	int *displs;
	sendcounts = (int *)malloc(numtasks*sizeof(int)); //data sent to each processor (remainder will be different)     
	displs = (int *)malloc(numtasks*sizeof(int)); //displacement from send_buff
	
	int sum =0; //distance from start of sendbuff = X 
	
	 int rem = SIZE % numtasks; //remainder 
	//printf(" ... rem  %d \n" ,rem);
	
	double local_meanX =0.0;
	double global_meanX = 0.0;
	double global_meanY = 0.0;
	double local_meanY =0.0;
	 
	
	double local_sdX = 0.0;
	double local_sdY = 0.0;
	double global_sdX = 0.0;
	double global_sdY = 0.0;

	double local_pXY = 0.0;
	
	double global_pXY = 0.0;
 
	

	for(int i =0; i<numtasks;i++) // this calculates the remainder cycles around and add remainder to the cores until no rem left . 
	{
		
		
			
		
		sendcounts[i] = sendcount_int;

		if(rem>0)
		{
			sendcounts[i]++;
			rem--;
		}
	   displs[i] = sum;
	   sum += sendcounts[i];

	 //printf(" int i : %d ,  sendcounts  :  %d ,  displacement %d :  \n ", i ,  sendcounts[i], displs[i]);
	}

	double *rec_buffx = (double *)malloc((sendcounts[rank]*sizeof(double))); //recieve_buff sent to all processes ;
	// + 1 to take into account of possible remainder SIZE % numtasks from sccatterv
	double *rec_buffy = (double *)malloc((sendcounts[rank]*sizeof(double)));


	double start, finish, loc_elapsed, elapsed ;
	if(rank==0)
	{

	
	
        X = (double *)malloc(SIZE*sizeof(double));
	
	Y = (double *)malloc(SIZE*sizeof(double));
	
	 dataInit(X,0,SIZE);
	 dataInit(Y,5,SIZE);
	 
	start = MPI_Wtime(); // start timing after init to keep consistency with the serial
	 


	MPI_Scatterv(X, sendcounts, displs, MPI_DOUBLE, rec_buffx, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatterv(Y, sendcounts, displs, MPI_DOUBLE, rec_buffy, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	 

	 //MPI_Scatter(X, sendcount, MPI_DOUBLE, rec_buffx, sendcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	 //MPI_Scatter(Y, sendcount, MPI_DOUBLE, rec_buffy, sendcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	  local_meanX = mean_func( rec_buffx , sendcounts[rank]);
	  local_meanY = mean_func( rec_buffy , sendcounts[rank]);
	
	
//-------------------GLOBAL--MEAN-------------------------
	  MPI_Allreduce(&local_meanX,&global_meanX,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

	  MPI_Allreduce(&local_meanY,&global_meanY,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
//---------------------------------------------------------	 
	  local_sdX = sd_func(rec_buffx,global_meanX,sendcounts[rank]);
	  local_sdY = sd_func(rec_buffy,global_meanY,sendcounts[rank]);
//--------------------Global sd--------------------------------
	  MPI_Reduce(&local_sdX,&global_sdX,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	  
	  MPI_Reduce(&local_sdY,&global_sdY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	  //-----------------------------------------------------
	 // printf("this is globalmX sum :  %.20lf , at rank %d ,\n " , global_meanX, rank  );
	//  printf("this is globalmY sum :  %lf , at rank %d , \n" , global_meanY, rank  );
	  
	local_pXY = pxy_func(rec_buffx, rec_buffy, sendcounts[rank], global_meanX, global_meanY);
	MPI_Reduce(&local_pXY,&global_pXY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	

	

	

	}
	else
	{
		MPI_Scatterv(X, sendcounts, displs, MPI_DOUBLE, rec_buffx, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Scatterv(Y, sendcounts, displs, MPI_DOUBLE, rec_buffy, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		//MPI_Scatter(X, sendcount, MPI_DOUBLE, rec_buffx, sendcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		//MPI_Scatter(Y, sendcount, MPI_DOUBLE, rec_buffy, sendcount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	   /*if(rank == 2)
		{
			//printf("SENDC2 = %d  \n ",sendcounts[2] );
			for(int i = 0; i < sendcounts[2]; i++)
			{
			//printf(" int i :  %d  ,   rec_buffx  : %.20lf  \n " , i , rec_buffx[i] );
			}
		}*/
				//------------------mean-----------------------------//
		local_meanX = mean_func( rec_buffx , sendcounts[rank]);
		local_meanY = mean_func( rec_buffy , sendcounts[rank]);
	   

		MPI_Allreduce(&local_meanX,&global_meanX,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
	MPI_Allreduce(&local_meanY,&global_meanY,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

		
		
		//--------------------------sd-----------------------------//

		local_sdX = sd_func(rec_buffx,global_meanX,sendcounts[rank]);
		local_sdY = sd_func(rec_buffy,global_meanY,sendcounts[rank]);


		MPI_Reduce(&local_sdX,&global_sdX,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
		
		MPI_Reduce(&local_sdY,&global_sdY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

	   //-------------------------pxy-----------------------------//
	   
	   local_pXY = pxy_func(rec_buffx, rec_buffy, sendcounts[rank], global_meanX, global_meanY);
	   MPI_Reduce(&local_pXY,&global_pXY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	   
		
	//	printf("this is globalsd sum :  %lf , at rank %d , \n" , global_sdX, rank  );
		//printf("this is globalsY sum :  %lf , at rank %d , \n " , global_sdY, rank  );
		
		

	}

	
	
	
	
	MPI_Reduce(&local_sdX,&global_sdX,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	
	if(rank==0)
	{
		
	//MPI_Reduce(&local_sdY,&global_sdY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	//MPI_Reduce(&local_pXY,&global_pXY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
		
            
	   	
	   
	    global_pXY = global_pXY/sqrt(global_sdX*global_sdY);
	   finish = MPI_Wtime();
	   printf("this is the time %.10f \n ", finish-start);

	   printf("this is global sum :  %.10lf , at rank %d , \n" ,global_pXY , rank  );

		
	  // File *infile;
	   //infile = fopen(char *str, "w");



	free(Y);

	free(X);
	}
	//printf("this is global sum :  %.20lf , at rank %d , \n" , global_meanX/(numtasks*SIZE), rank  );
        
	free(rec_buffx);
	free(rec_buffy);
	
	free(sendcounts);
	free(displs);
	
	MPI_Finalize();
}
