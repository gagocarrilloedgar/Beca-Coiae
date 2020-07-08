#include "headers.h"
int comprobar(int N, int M1, double M[],double R[],int heigth)
{
  double ok2 = N*rank() + N + 1;
  double ok3 = N*rank() + N -N;
      if (rank()==0)
      {
        if (M[0]==0 && (M[N+1] == ok2 ))
        {
           return 1;
        }
        else{ return 0;}

      }
      if (rank() == commsize()-1)
      {
        if (M[N+1]==0 && (M[0] ==ok3 ))
        {
          return 1;
        }
        else
        {
          return 0;
        }
      }
      else
      {
        if (M[0]==ok3 && ( M[N+1] == ok2 ))
        {
          return 1 ;
        }
        else
        {
          return 0;
        }
      }
}

int fcheck(int i) {
		return(i*100);
}
