#include "headers.h"

void lambda(double *x,int N,double *T,double k, int op)
{
	if (op==1) // w
	{
		for (int i =1; i<N; ++i)
		{
			x[i-1]=k*(T[i]+T[i-1])/2;
		}
	}

	if (op==2) //e
	{
		for (int i = 0; i <N; ++i)
		{
			x[i]=k*(T[i]+T[i+1])/2;
		}
	}

}
