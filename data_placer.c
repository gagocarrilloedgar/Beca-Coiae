#include "headers.h"
#include "mpi.h"

void data_placing(double M[],double R[],int mystart,int myend,int N,int heigth){

	int h=1;
	if (commsize()%2==0)
	{
		if (rank()%2==0)
		{
				M[heigth+1] = R[0];
		}
		else
		{
				M[0]=R[0];
		}
	}

	else if (commsize()%2!=0)
	{
		if (rank()%2==0 && rank()!=(commsize()-1))
		{
					*(M+heigth+1)=*(R+0);

		}

		else if(rank()%2!=0)
		{
				*(M+0)=*(R+0);
		}
	}


}


void data_placing2(double M[],double R[],int mystart,int myend,int N,int heigth){

	int h=1,end=myend,start=mystart;

	if (commsize()%2==0)
	{
		if (rank()%2==0 && rank()!=0)
		{
					*(M+0)=*(R+0);
		}

		else if( rank()%2!=0 && rank()!=(commsize()-1))
		{
					*(M+heigth+1)=*(R+0);
		}
	}

	else if (commsize()%2!=0)
	{
		if (rank()%2==0 && rank()!=0)
		{
					*(M+0)=*(R+0);
		}

		else if( rank()%2!=0)
		{
			*(M+heigth+1)=*(R+0);
		}
	}


}
