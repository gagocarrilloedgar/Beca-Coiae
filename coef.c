#include "headers.h"
#include "mpi.h"
void vectori(int N,double Tin[],double Tinincial){

	for (int i = 0; i<N; ++i)
    	{
    		*(Tin+i)=Tinincial;

    	}

}

void coef0(double a[],double deltat, int N,double rho, double vp[],double cp)//apn
{
	for (int i =0; i <N; ++i)
	{
		a[i]=(rho*vp[i]*cp)/(deltat);
	}

}

void coef1(double a[],int N,double S[],double delta,double lamda[],double beta,int op){ // aw and ae

	if (op==1)
	{
		for (int i=0; i <N; ++i) // empezamos en 1 ya que depués tendremos que aplicar condiciones de contorno
		{
			*(a+i)=(beta*lamda[i]*S[i])/delta;
		}
	}
	if (op==2)
	{
		for (int i =0; i <N; ++i) // empezamos en 1 ya que depués tendremos que aplicar condiciones de contorno
		{
			*(a+i)=(1-beta)*lamda[i]*S[i]/delta;
		}

	}

}

void coef2(double a[],int N,double T,double alpha, double A[],double aw[],double ae[],double *Tin,double *ap,double beta){ // bp
int init;

	for (int i = 0; i <N-1; ++i)
	{
		*(a+i)=(-1)*A[i]*T*alpha + aw[i]*Tin[i-1] + ae[i]*Tin[i+1] + ap[i]*Tin[i];
	}
}

void coef2_2(double a[],int N,double T,double alpha, double A[],double S[],double aw[],double *Tin,double *ap,double beta)
{

	*(a+N-1)=-1*(A[N-1])*T*alpha + aw[N-1]*Tin[N-2]+ap[N-1]*Tin[N-1]+(1-beta)*(Tin[N-1]-T)*alpha*S[N-1] ;
	return;
}

void coef3(double a1[],double a2[],double a[],int N,double beta,double a3[]){ //ap
	for (int i = 0; i <N; ++i)
	{
		*(a+i)=*(a1+i)+*(a2+i)+beta*a3[i];
	}

}
