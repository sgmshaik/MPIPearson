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


int main(int argc, char *argv[])
{
	

	double *X;
	double *Y;
	int size  = atoi(argv[1]);
	//printf("size %d  : \n", size);
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
	//printf("This is pxy : %lf and the time is : %lf  \n" , pxy, elapsedt);

	FILE *infile;
	
	infile = fopen("s_res.dat","a");

	if(infile != NULL)
	{
	   //c1 number of processes, c2 size of array , c2 time		
	   fprintf(infile, "%10d		%10d		%20lf \n", 1 ,size,elapsedt );
	   
	   fclose(infile);
	   
   }else{printf(" ERROR \n");}

free(X);
free(Y);
return 1;
}
