#include "headers.h"

void halo_update(double M[],double S[],double R[],int N, int  myend, int mystart,int heigth)
{	
  // primer ciclo de comunicación
  data_to_send(N,M,S,mystart,myend,heigth);
  halo1(R,S,N);
  data_placing(M,R,mystart,myend,N,heigth);

  MPI_Barrier(MPI_COMM_WORLD);// esperamos a que todos lso procesadores hayan completado el ciclo

  //segundo ciclo de comunicación
  data_to_send2(N,M,S,mystart,myend,heigth);
  halo2(R,S,N);
  data_placing2(M,R,mystart,myend,N,heigth);

  MPI_Barrier(MPI_COMM_WORLD); // volvemos a esperar que todos han acabado antes de finalizar
}
