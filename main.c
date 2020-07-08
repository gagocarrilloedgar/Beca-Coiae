#include "headers.h"
#define PI 3.14159265358979323846;
#include "mpi.h"

int main(int argc, char** argv)
{
    struct initial_temperatures init_temp;
    struct properties prop;
    struct control cont;
    struct geometry geo;

    struct vectors *e,*p,*w;
    struct control_point *point;
    struct temperatures *Temp;
    struct send *halo;
    struct limits my;

    checkr(MPI_Init(&argc,&argv),"init");

    if (rank()==0)
    {
        /*
        prop=prop_input();
        init_temp=temp_input();
        cont=control_input();
        geo=geometry_input();*/

    init_temp.Twall=126.5;
    init_temp.Text=10;
    init_temp.Textr=10;
    init_temp.T0=10;
    init_temp.T1=4;

    prop.rho=200;
    prop.cp=10;
    prop.alphaexte=10;
    prop.alphaextr=10;
    prop.lambdaf=200;

    cont.N=5000;
    cont.deltat=0.1;
    cont.delta=0.01;
    cont.beta=0.5;

    geo.Ra=0.02;
    geo.Rb=0.1;
    geo.ef=0.001;
  }

    // first we need to prepare the information we are about to send

    int count=2; // cause we have int and double
    //Says the type of every block
    MPI_Datatype array_of_types_cont[count];
    MPI_Datatype array_of_types_prop[count-1];
    MPI_Datatype array_of_types_temp[count-1];
    MPI_Datatype array_of_types_geo[count-1];
    //We have int and douhble
    array_of_types_cont[0] = MPI_INT;
    array_of_types_cont[1] = MPI_DOUBLE;
    array_of_types_prop[0] = MPI_DOUBLE;
    array_of_types_temp[0] = MPI_DOUBLE;
    array_of_types_geo[0] = MPI_DOUBLE;

    //Says how many elements for block
    int array_of_blocklengths_prop[count-1];
    int array_of_blocklengths_cont[count];
    int array_of_blocklengths_geo[count-1];
    int array_of_blocklengths_temp[count-1];
    //we have 4 int & 2 double
    array_of_blocklengths_cont[0] = 1;
    array_of_blocklengths_cont[1] = 4;
    array_of_blocklengths_prop[0] = 5;
    array_of_blocklengths_geo[0] = 3;
    array_of_blocklengths_temp[0] = 5;

    /*Says where every block starts in memory, counting from the beginning of the struct.*/
    MPI_Aint array_of_displaysments_prop[count-1];
    MPI_Aint array_of_displaysments_cont[count];
    MPI_Aint array_of_displaysments_temp[count-1];
    MPI_Aint array_of_displaysments_geo[count-1];

    MPI_Aint address1_cont,address1_prop,address1_temp,address1_geo;
    MPI_Aint address2_cont,address2_prop,address2_temp,address2_geo;
    MPI_Aint address3_cont;

    MPI_Get_address(&cont,&address1_cont);
    MPI_Get_address(&prop,&address1_prop);
    MPI_Get_address(&init_temp,&address1_temp);
    MPI_Get_address(&geo,&address1_geo);
    MPI_Get_address(&cont.N,&address2_cont);
    MPI_Get_address(&prop.rho,&address2_prop);
    MPI_Get_address(&init_temp.Twall,&address2_temp);
    MPI_Get_address(&geo.Ra,&address2_geo);
    MPI_Get_address(&cont.deltat,&address3_cont);

    array_of_displaysments_cont[0] = address2_cont - address1_cont;
    array_of_displaysments_cont[1] = address3_cont - address1_cont;
    array_of_displaysments_prop[0] = address2_prop - address1_prop;
    array_of_displaysments_geo[0] = address2_geo - address1_geo;
    array_of_displaysments_temp[0] = address2_temp - address1_temp;

    /*Create MPI Datatype and commit*/
    MPI_Datatype stat_type_cont,stat_type_prop,stat_type_temp,stat_type_geo;

    MPI_Type_create_struct(count, array_of_blocklengths_cont, array_of_displaysments_cont, array_of_types_cont, &stat_type_cont);
    MPI_Type_commit(&stat_type_cont);
    MPI_Type_create_struct(count-1, array_of_blocklengths_prop, array_of_displaysments_prop, array_of_types_prop, &stat_type_prop);
    MPI_Type_commit(&stat_type_prop);
    MPI_Type_create_struct(count-1, array_of_blocklengths_temp, array_of_displaysments_temp, array_of_types_temp, &stat_type_temp);
    MPI_Type_commit(&stat_type_temp);
    MPI_Type_create_struct(count-1, array_of_blocklengths_geo, array_of_displaysments_geo, array_of_types_geo, &stat_type_geo);
    MPI_Type_commit(&stat_type_geo);

	//1. input data definiton

    if (rank()==0)
    {
        for (int i = 0; i < commsize(); ++i)
        {
            MPI_Send(&cont, 1, stat_type_cont, i, 0, MPI_COMM_WORLD);
            MPI_Send(&prop, 1, stat_type_prop, i, 0, MPI_COMM_WORLD);
            MPI_Send(&geo, 1, stat_type_geo, i, 0, MPI_COMM_WORLD);
            MPI_Send(&init_temp, 1, stat_type_temp, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&cont,1,stat_type_cont,0,0,MPI_COMM_WORLD,NULL);
        MPI_Recv(&prop,1,stat_type_prop,0,0,MPI_COMM_WORLD,NULL);
        MPI_Recv(&geo,1,stat_type_geo,0,0,MPI_COMM_WORLD,NULL);
        MPI_Recv(&init_temp,1,stat_type_temp,0,0,MPI_COMM_WORLD,NULL);
    }

    // printf(" cont.N=%d \n",cont.N); --> works good

	// --------------------Dynamic space for matrix and vector------------------------------------------------------
	// Vectors

    double timet = MPI_Wtime();
    worksplit(0,cont.N,commsize(),rank(),&my.start,&my.end,&my.width);
  //  printf("mystart=%d myend=%d mywidth=%d rank=%d \n",my.start,my.end,my.width,rank());

    e=(struct vectors*)malloc(sizeof(struct vectors));
        e->S=(double *)malloc(sizeof(double)*my.width);
        e->r=(double *)malloc(sizeof(double)*my.width);
        e->lambda=(double *)malloc(sizeof(double)*my.width);
        e->lambdan=(double *)malloc(sizeof(double)*my.width);
        e->a=(double *)malloc(sizeof(double)*(my.width+2));
        e->an=(double *)malloc(sizeof(double)*my.width);

        if (e->S==NULL || e->r==NULL || e->lambda==NULL || e->lambdan==NULL || e->a==NULL || e->an==NULL)
        {
            printf("Out of memory\n");
            exit(1);
        }

     w=(struct vectors*)malloc(sizeof(struct vectors));
        w->S=(double *)malloc(sizeof(double)*my.width);
        w->r=(double *)malloc(sizeof(double)*my.width);
        w->lambda=(double *)malloc(sizeof(double)*my.width);
        w->lambdan=(double *)malloc(sizeof(double)*my.width);
        w->a=(double *)malloc(sizeof(double)*(my.width+2));
        w->an=(double *)malloc(sizeof(double)*my.width);

        if (w->S==NULL || w->r==NULL || w->lambda==NULL || w->lambdan==NULL || w->a==NULL || w->an==NULL)
        {
            printf("Out of memory\n");
            exit(1);
        }

     p=(struct vectors*)malloc(sizeof(struct vectors));
        p->S=(double *)malloc(sizeof(double)*my.width);
        p->r=(double *)malloc(sizeof(double)*my.width);
        p->lambda=(double *)malloc(sizeof(double)*my.width);
        p->lambdan=(double *)malloc(sizeof(double)*my.width);
        p->a=(double *)malloc(sizeof(double)*(my.width+2));
        p->an=(double *)malloc(sizeof(double)*my.width);

        if (p->S==NULL || p->r==NULL || p->lambda==NULL || p->lambdan==NULL || p->a==NULL || p->an==NULL)
        {
            printf("Out of memory\n");
            exit(1);
        }

    // Control points
    point=(struct control_point*)malloc(sizeof(struct control_point));
        point->vp=(double *)malloc(sizeof(double)*my.width);
        point->Ap=(double *)malloc(sizeof(double)*my.width);
        point->bp=(double *)malloc(sizeof(double)*my.width);

        if (point->vp==NULL || point->Ap==NULL || point->bp==NULL)
        {
            printf("Out of memory\n");
            exit(1);
        }

    // Temperatures
    Temp=(struct temperatures *)malloc(sizeof(struct temperatures));
        Temp->T=(double *)malloc(sizeof(double)*(cont.N));
        Temp->Tin=(double *)malloc(sizeof(double)*(cont.N));
        Temp->Tfut=(double *)malloc(sizeof(double)*(cont.N));

         if (Temp->T==NULL || Temp->Tin==NULL || Temp->Tfut==NULL)
        {
            printf("Out of memory\n");
            exit(1);
        }

    //Halo vectors
    halo=(struct send *)malloc(sizeof(struct send));
        halo->S=(double *)malloc(sizeof(double)*1);
        halo->R=(double *)malloc(sizeof(double)*1);

        if (halo->S==NULL || halo->R==NULL)
        {
            printf("Out of memory\n");
            exit(1);
        }
  //--------------------- comprovación del algoritmo de comunicaciones --------------------------
    //printf("rank=%d ", rank());
    for (int i = 1; i<=my.width; ++i)
    {
      *(Temp->T+i)=my.width*rank()+i;
    }

  halo_update(Temp->T,halo->S,halo->R,my.width,my.start,my.end,my.width);

  if(comprobar(my.width,1,Temp->T,halo->R,cont.N)==1)
  {
    printf("Test passed \n" );
  }
  else
  {
    printf("Test failed \n" );
  }

	//----------------------------2.Cálculos previos (Geometria)--------------------------------
    cont.deltar=(double)(geo.Rb-geo.Ra)/(cont.N-1);
    linspace(geo.Ra,geo.Rb,my.start,my.end,cont.N,w->r,1); //rw
    linspace(geo.Ra,geo.Rb,my.start,my.end,cont.N,p->r,2); //r
    linspace(geo.Ra,geo.Rb,my.start,my.end,cont.N,e->r,3); //re

    Area(geo.ef,w->r,w->S,my.width);//Sw
    Area(geo.ef,e->r,e->S,my.width);//Se
    Area(geo.ef,p->r,p->S,my.width);//S
    perimetro(e->r,w->r,point->Ap,my.width);//Ap
    vol(point->vp,p->S,geo.Rb-geo.Ra,my.width);//Vp
    if (rank()==commsize()-1)
    {
      *(p->r+my.width-1)=geo.Rb;
    }

    // 3. Initial Temperatures map
    vectori(my.width+2,Temp->Tin,init_temp.T0); //--> Tn
    vectori(my.width+2,Temp->Tfut,init_temp.T1);// T(n+1)*

    //4. Coeficientes de discretización
    lambda(e->lambda,my.width,Temp->Tfut,prop.lambdaf,2);
    lambda(w->lambda,my.width,Temp->Tfut,prop.lambdaf,1);

    // _n dependen de las condiciones iniciales no se actualizan para un delta de t
    lambda(e->lambdan,my.width,Temp->Tin,prop.lambdaf,2);
    lambda(w->lambdan,my.width,Temp->Tin,prop.lambdaf,1);
    coef0(p->an,cont.deltat,my.width,prop.rho,point->vp,prop.cp);

    coef1(w->a,my.width,w->S,cont.deltar,w->lambda,cont.beta,1);//aw
    coef1(e->a,my.width,e->S,cont.deltar,e->lambda,cont.beta,1);//ae


    coef1(w->an,my.width,w->S,cont.deltar,w->lambdan,cont.beta,2);//awn // no se actualizan
    coef1(e->an,my.width,e->S,cont.deltar,e->lambdan,cont.beta,2);//aen // no se actualiza en este delata de t

    coef2(point->bp,my.width,init_temp.Text,prop.alphaexte,point->Ap,w->an,e->an,Temp->Tin,p->an,cont.beta);//bp
    coef2_2(point->bp,my.width,init_temp.Text,prop.alphaexte,point->Ap,p->S,w->an,Temp->Tin,p->an,cont.beta);//bpn

    if (cont.beta!=0)
    {
      coef3(w->a,e->a,p->a,my.width,cont.beta,p->an); //ap
    }

    else
    {
      coef3(w->an,e->an,p->a,my.width,cont.beta,p->an); //ap
    }
    //condiciones de contorno(i=0)--> primer elemento
    if (rank()==0)
    {
    *(p->a+0)=1;
    *(e->a+0)=0;
    *(point->bp+0)=init_temp.Twall;
    *(w->a+0)=0;
    }
    //condiciones de contorno(i=N+1) --> último elemento
    if (rank()==commsize()-1)
    {
        *(e->a+my.width-1)=0;
    }
    // 5 Aplicación de GAUSS-SEIDEL
    if (cont.beta !=0)
    {
      temp(Temp->T,Temp->Tfut,my.width,p->a,e->a,w->a,point->bp);
    }
    else
    {
      temp(Temp->T,Temp->Tin,my.width,p->a,e->a,w->a,point->bp);
    }
    //condiciones de contorno de T
    if (rank()==0)
    {
        *(Temp->T+1)=init_temp.Twall;
    }
    //---------------------------------------------HALO_UPDATE_T-------------------------------------
    halo_update(Temp->T,halo->S,halo->R,my.width,my.start,my.end,my.width);

    //--------------------------------------------HALO_UPDATE_T------------------------------------

    double *ok_max;
    ok_max=(double *)malloc(sizeof(double)*commsize());

    double ok;
    ok=max(Temp->T,Temp->Tfut,my.width,cont.delta);
   	comm_max(ok,ok_max);

    int salir=0,salir2=0;

    if (rank()==0)
    {
         salir=max_ok(ok_max,cont.delta);
    }

    enviar(salir,&salir2);

if (cont.beta!=0)
{
  while(salir2==0){
      for (int i =1; i<=my.width; ++i)
      {
          *(Temp->Tfut+i)=*(Temp->T+i);
      }
      //4. Coeficientes de discretización
      lambda(e->lambda,my.width,Temp->Tfut,prop.lambdaf,2);
      lambda(w->lambda,my.width,Temp->Tfut,prop.lambdaf,1);
      coef1(w->a,my.width,w->S,cont.deltar,w->lambda,cont.beta,1);//aw
      coef1(e->a,my.width,e->S,cont.deltar,e->lambda,cont.beta,1);//ae
      coef2(point->bp,my.width,init_temp.Text,prop.alphaexte,point->Ap,w->an,e->an,Temp->Tin,p->an,cont.beta);//bp
      coef2_2(point->bp,my.width,init_temp.Text,prop.alphaexte,point->Ap,p->S,w->an,Temp->Tin,p->an,cont.beta);//bpn
      coef3(w->a,e->a,p->a,my.width,cont.beta,p->an); //ap
      if (rank()==0)
      {
        *(p->a+0)=1;
        *(e->a+0)=0;
        *(point->bp+0)=init_temp.Twall;
        *(w->a+0)=0;
      }
      else if (rank()==commsize()-1)
      {
          *(e->a+my.width-1)=0;
      }
      //condiciones de contorno(i=N+1) --> último element
      temp(Temp->T,Temp->Tfut,my.width,p->a,e->a,w->a,point->bp);
      //condiciones de contorno de T
   //---------------------------------------------HALO_UPDATE_T-------------------------------------
   halo_update(Temp->T,halo->S,halo->R,my.width,my.start,my.end,my.width);
  //--------------------------------------------HALO_UPDATE_T------------------------------------
  ok=max(Temp->T,Temp->Tfut,my.width,cont.delta);
  comm_max(ok,ok_max);
  if (rank()==0)
  {
      salir=max_ok(ok_max,cont.delta);
  }
  enviar(salir,&salir2);
  }
}

if(cont.beta==0)
{
  while(salir2==0){
      for (int i =1; i<=my.width; ++i)
      {
          *(Temp->Tin+i)=*(Temp->T+i);
      }
      //4. Coeficientes de discretización
      lambda(e->lambdan,my.width,Temp->Tin,prop.lambdaf,2);
      lambda(w->lambdan,my.width,Temp->Tin,prop.lambdaf,1);
      coef1(w->an,my.width,w->S,cont.deltar,w->lambdan,cont.beta,2);//awn // no se actualizan
      coef1(e->an,my.width,e->S,cont.deltar,e->lambdan,cont.beta,2);//aen // no se actualiza en este delata de t
      coef2(point->bp,my.width,init_temp.Text,prop.alphaexte,point->Ap,w->an,e->an,Temp->Tin,p->an,cont.beta);//bp
      coef2_2(point->bp,my.width,init_temp.Text,prop.alphaexte,point->Ap,p->S,w->an,Temp->Tin,p->an,cont.beta);//bpn
      coef3(w->an,e->an,p->a,my.width,cont.beta,p->an); //ap
      if (rank()==0)
      {
        *(p->an+0)=1;
        *(e->an+0)=0;
        *(point->bp+0)=init_temp.Twall;
        *(w->an+0)=0;
      }
      if (rank()==commsize()-1)
      {
        *(e->an+my.width-1)=0;
      }
      //condiciones de contorno(i=N+1) --> último element
      temp(Temp->T,Temp->Tin,my.width,p->a,e->a,w->a,point->bp);
      //condiciones de contorno de T
   //---------------------------------------------HALO_UPDATE_T-------------------------------------
   halo_update(Temp->T,halo->S,halo->R,my.width,my.start,my.end,my.width);
  //--------------------------------------------HALO_UPDATE_T------------------------------------
  ok=max(Temp->T,Temp->Tin,my.width,cont.delta);
  comm_max(ok,ok_max);
  if (rank()==0)
  {
      salir=max_ok(ok_max,cont.delta);
  }
  enviar(salir,&salir2);
  }
}
if(rank()==0)
{
  printf("\n time=%f \n", MPI_Wtime()-timet );
}
    /* for (int i =0; i <=my.width+1; ++i)
    {
        printf("temp=%f rank=%d i=%d \n ",Temp->T[i],rank(),i);
    }*/

   // Now we free the space we've reserved at the begining
    free(e->r); free(e->S); free(e->lambda); free(e->lambdan); free(e->a); free(e->an); free(e);
    free(w->r); free(w->S); free(w->lambda); free(w->lambdan); free(w->a); free(w->an); free(w);
    free(p->r); free(p->S); free(p->lambda); free(p->lambdan); free(p->a); free(p->an); free(p);

   free(point->vp); free(point->Ap); free(point->bp);

   free(Temp->T); free(Temp->Tin); free(Temp->Tfut);
   // free the space from the structures we've passed before
   MPI_Type_free(&stat_type_cont);
   MPI_Type_free(&stat_type_temp);
   MPI_Type_free(&stat_type_geo);
   MPI_Type_free(&stat_type_prop);


   crash();
}
