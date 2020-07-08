#include "headers.h"
#include "mpi.h"

void linspace(double Ra, double Rb, int start,int end,int N, double r[],int op)
{
	double c=0.0,k=0.0;
	c=(Rb - Ra)/((double)N-1);
	int width=end-start+1;


	for (int i =0; i <width; ++i)
	{
		k= Ra+(double)(i+start)*(c);

		if (op==1)
			*(r+i)=k-c/2;
		if (op==3)
			*(r+i)=k+c/2;

		if (op==2)
			*(r+i)=k;

	}


}
