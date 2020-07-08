#include "headers.h"
#include "mpi.h"
struct properties prop_input()
{
	struct properties prop;
	printf("Properties data: Cp alphaext alphaextr lambdaf rho\n");
	scanf("%lf %lf %lf %lf %lf",&prop.cp,&prop.alphaexte,&prop.alphaextr,&prop.lambdaf,&prop.rho);
	fflush(stdin);
	return prop;
};

struct initial_temperatures temp_input()
{
	struct initial_temperatures init_temp;
	printf("Iinitial temperatures data: Twall Text Textr T0 T1\n");
	scanf("%lf %lf %lf %lf %lf",&init_temp.Twall,&init_temp.Text,&init_temp.Textr,&init_temp.T0,&init_temp.T1);
	fflush(stdin);
	return init_temp;
};


struct control control_input()
{
	struct control cont;

	printf(" control Data: deltat delta beta N \n ");
	scanf("%lf %lf %lf %i",&cont.deltat,&cont.delta,&cont.beta,&cont.N);
	fflush(stdin);
	return cont;

};

struct geometry geometry_input()
{
	struct geometry geo;
	printf("Goemtry Data: Ra Rb ef\n");
	scanf("%lf %lf %lf",&geo.Ra,&geo.Rb,&geo.ef);
	return geo;
};
