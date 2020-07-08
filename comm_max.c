
#include "headers.h"

void comm_max(double ok, double max[])
{

	 if (rank()!=0)
    {
        checkr(MPI_Send(&ok,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD),"sending" );
    }

    else if(rank()==0)
    {
            max[0]=ok;
            for (int i = 1; i < commsize(); ++i)
            {
                checkr(MPI_Recv(&ok,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,NULL),"reciving");
                max[i]=ok;
            }
    }

}

void enviar(int salir,int *salir2)
{
	if (rank()==0)
	{
		*salir2=salir;
		for (int i = 0; i < commsize(); ++i)
		{
			checkr(MPI_Send(&salir,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD),"sending" );
		}
	}
	else
	{
		checkr(MPI_Recv(&salir,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD,NULL),"reciving");
		*salir2=salir;
	}

}
