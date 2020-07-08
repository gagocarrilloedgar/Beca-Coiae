#include "headers.h"
#include "mpi.h"
void temp(double T[],double Tfut[],int N,double ap[],double ae[],double aw[],double bp[] )
{
		for (int i =1; i <=N; ++i)
    	{
    		*(T+i)=(ae[i-1]*Tfut[i+1]+aw[i-1]*Tfut[i-1]+bp[i-1])/ap[i-1];
    	}
}

double max(double T[],double Tin[],int N, double delta ){

	double max=0.0,x=0;

	for (int i = 1; i <=N; ++i)
	{
		x=fabs(T[i]-Tin[i]);

		if(x>max){
			max=x;
		}
	}
	//printf("%f\n",max ); quita el comentario para ver el maximo de cada procesador


		return max;
}

int max_ok(double max[],double delta)
{
	double x=0;
	x=max[0];
	for (int i = 0; i < commsize(); ++i)
	{
		if (max[i]>x)
		{
			x=max[i];
		}
	}
	//printf("maximo final = %f \n",x ); para ver el maximo global quita el comentario de la linea

	if (x<delta)
	{
		return 1;
	}

	else
		return 0;
}
