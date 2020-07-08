#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <strings.h>

#define PI 3.14159265358979323846;

//----------------------------------------HEAT-COND-FUNCTIONS-----------------------------------------------
// Data inputs
struct properties prop_input();
struct initial_temperatures temp_input();
struct control control_input();
struct geometry geometry_input();

// suprface and volum
void linspace(double a, double b,int start,int end,int N, double p[],int op);
void Area(double ef, double r[],double S[],int N);
void perimetro(double r[],double r1[],double A[],int N);
void vol(double V[],double A[],double Ra,int N);

//Coefficients and lambda
void coef0(double a[],double deltat, int N,double rho, double vp[],double cp);
void coef1(double a[],int N,double S[],double delta,double *lamda,double beta,int op);
void coef2(double a[],int N,double T,double alpha, double A[],double aw[],double ae[],double *Tin,double *ap,double beta);
void coef2_2(double a[],int N,double T,double alpha, double A[],double S[],double aw[],double *Tin,double *ap,double beta);
void coef3(double a1[],double a2[],double a[],int N,double beta,double a3[]);
void lambda(double *x,int N,double *T,double k,int op);

// Temperatures
void vectori(int N,double Tin[],double Tinincial);
void temp(double T[],double Tin[],int N,double ap[],double ae[],double aw[],double bp[]);
double max(double T[],double Tin[],int N, double delta );
int max_ok(double max[],double delta);


//----------------------------------------------MPI functions-------------------------------------------
// Common mpi functions
void crash();
void checkr(int r,char *txt);
int commsize();
int rank();

//Worksplit function
int worksplit(int start, int end,int P,int whoami,int *mystart, int *myend,int *mywidth);

// MPI communication function
void halo_update(double M[],double S[],double R[],int N, int  myend, int mystart,int heigth);
int halo1(double R[],double S[], int N);
int halo2(double R[],double S[], int N);

void data_to_send(int N, double M[],double R[],int mystart,int myend,int heigth);
void data_to_send2(int N, double M[],double R[],int mystart,int myend,int heigth);

void data_placing(double M[],double R[],int mystart,int myend,int N,int heigth);
void data_placing2(double M[],double R[],int mystart,int myend,int N,int heigth);

// Evaluación de maximos en paralelo
void comm_max(double ok, double max[]);
void enviar(int salir,int *salir2);

// Comprovación del HAlO
int fcheck(int i);
int comprobar(int N, int M, double S[],double R[],int heigth);


// --------------------------------------------DATA STRUCTURE DEFINITION--------------------------------------

struct initial_temperatures //init_temp
{
	double Twall,Text,Textr,T0, T1; // T1 to initialize Tin and T0 will be the intital map of temp

};

struct properties //prop
{
	double rho,cp,alphaextr, alphaexte,lambdaf;
	// lamda of the fin
	// density

};

struct control //cont
{
	int N;
	double deltat, deltar,delta,beta;

};

struct geometry //geo
{
	double Ra,Rb,ef;
	// initial and ending radius
	// fin thickness
};

struct vectors //e,p,w
{
	double *S;
	double *r;
	double *lambda,*lambdan;
	double *a;
	double *an;
};

struct control_point//point
{
	double *vp;
	double *Ap;
	double *bp;
};

struct temperatures//Temp
{
	double *T,*Tin,*Tfut;

};

struct send
{
	double *S,*R;
};

struct limits
{
	int start;
	int end;
	int width;
};
