#include "headers.h"
#include "mpi.h"
// this halo update only applies for one direction wich in this case is i, for 2D halo look for at github @gagocarrilloedgar
void data_to_send(int N, double M[],double S[],int mystart,int myend,int heigth){

	int h=1;

	if(commsize()%2==0)
	{
		if (rank()%2==0)
		{
			for (int i = 0; i < h; ++i)
			{
				*(S+(i))=*(M+(i+heigth));

			}
		}
		else if(rank()%2!=0)
		{

			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+(i+1));
			}
		}
	}

	else if(commsize()%2!=0)
	{
		if (rank()%2==0 && rank()!=(commsize()-1))
		{

			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+(i+heigth));
			}

		}
		else if(rank()%2!=0)
		{

			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+(i+1));
			}

		}
	}

}

void data_to_send2(int N, double M[],double S[],int mystart,int myend,int heigth){

	int h=1;

	if (commsize()%2==0)
	{
		if (rank()%2==0 && rank()!=0)
		{

			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+(i+1));
			}

		}
		else if (rank()%2!=0 && rank()!=(commsize()-1))
		{

			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+(i+heigth));
			}
		}
	}

	else if (commsize()%2!=0)
	{
		if (rank()%2==0 && rank()!=0)
		{
			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+i+1);
			}
		}

		else if (rank()%2!=0)
		{
			for (int i = 0; i < h; ++i)
			{
				*(S+i)=*(M+i+heigth);
			}
		}
	}

}
