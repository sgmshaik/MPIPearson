
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
//#define SIZE 240000000 going to define it from argv
int SIZE;


 double mean_func(double *data, int size)
{

	double mean = 0.0;
	for(int i = 0; i < size ; i++)
	{
		
	mean += data[i]; 
	
	}
	
	mean /= (double)SIZE; //global size helps simplify mean .

	
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


double pxy_serial()
{

	double *X;
	double *Y;
	int size  = SIZE;
	//printf("size %d  : \n", size);
	int shift = 5;
	
	X = (double *)malloc(size*sizeof(double));
	Y = (double *)malloc(size*sizeof(double));
	
	dataInit(X,0,size);	
	dataInit(Y,5,size);


	clock_t start, finish;
	
	start = clock();

	double meanX = mean_func(X,size);
	double meanY = mean_func(Y,size);
	double global_sdX= sd_func(X,meanX,size);
	double global_sdY= sd_func(Y,meanY,size);
	
	double pxy = pxy_func(X,Y,size,meanX,meanY)/(sqrt(global_sdX*global_sdY));
	finish = clock();	
	double elapsedt = (double)(finish - start)/ CLOCKS_PER_SEC; 
	printf("This is serial pxy : %lf and the time is : %lf  \n" , pxy, elapsedt);

	/*FILE *infile;
	
	infile = fopen("s_res.dat","a");

	if(infile != NULL)
	{
	   //c1 number of processes, c2 size of array , c2 time		
	   fprintf(infile, "%10d		%10d		%20lf \n", 1 ,size,elapsedt );
	   
	   fclose(infile);
	   
   }else{printf(" ERROR \n");}*/
free(X);
free(Y);
return elapsedt;


}

double pxy_mpi(int numtasks, int rank, int sendcount_int,int recvcount)
{

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
	double elapsedt=0.;
	if(rank==0)
	{
		
	//MPI_Reduce(&local_sdY,&global_sdY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	//MPI_Reduce(&local_pXY,&global_pXY,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
		
            
	   	
	   
	    global_pXY = global_pXY/sqrt(global_sdX*global_sdY);
	   finish = MPI_Wtime();
	    elapsedt = finish-start; 
	   printf("this is the time %.10f \n ", elapsedt);

	   printf("this is globalpxy :  %.10lf , at rank %d , for SIZE %d , and number of processes %d \n" ,global_pXY , rank , SIZE, numtasks );

		
		//File *infile;
		  //char filename[100];
		 //sprintf(filename,"p_res",numtasks,SIZE,finish-start);
	     // infile = fopen(filename, "a");

		 
		 //sprintf(line, " -----------N------------SIZE---------------TIME");
		/* FILE *infile;
		 
		 infile = fopen("p_res.dat","a");

		 if(infile != NULL)
		 {
			//c1 number of processes, c2 size of array , c2 time		
			fprintf(infile, "%10d		%10d		%20lf \n", numtasks ,SIZE,(finish-start) );
            
			fclose(infile);
		    
		}else{printf(" ERROR FILE \n");}*/
	free(Y);

	free(X);
	}
        
	free(rec_buffx);
	free(rec_buffy);
	
	free(sendcounts);
	free(displs);
    return elapsedt;
	
}



int main(int argc, char *argv[])
{
	

	int numtasks, rank, sendcount_int, recvcount;
	double ptime,stime;
    MPI_Init(NULL, NULL);
    SIZE = atoi(argv[1]); // converts char[0] to SIZE is a global variable .
    //printf("SIZE : %d \n ", SIZE);
	/*if(argc == 1)
	{
		
	} */  

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	
	if(rank==0)
	{
     stime = pxy_serial();
	}

	if(numtasks>1)
	 ptime = pxy_mpi( numtasks, rank,sendcount_int, recvcount);
	
	if (rank == 0)
	{
	 
		printf("\n SPEEDUP ACHIEVED: %lf\n\n", ((stime/ptime)));
		

	}
   
	
	
	MPI_Finalize();
return 0;
}
