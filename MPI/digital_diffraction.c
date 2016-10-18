/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

typedef struct {
  int Nsources, Nx, Ny;
  double dist, x1, y1, x2, y2, lambda;
  struct { double x, y, a; } *sources;
} diff_data_t;

void * secure_malloc(size_t size) {
  void * p=malloc(size);
  if (p==NULL)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  return p;
}

diff_data_t read_input(const char *fname) {
  int i;
  diff_data_t D={0};
  FILE *fd;
  fd=fopen(fname, "r");
  if (fd!=NULL) {
    fscanf(fd, "dist%lf\n", &D.dist);
    fscanf(fd, "screen%lf%lf%lf%lf\n", &D.x1, &D.y1, &D.x2, &D.y2);
    fscanf(fd, "picture%i%i\n", &D.Nx, &D.Ny);
    fscanf(fd, "lambda%lf\n", &D.lambda);
    fscanf(fd, "sources%i\n", &D.Nsources);
    D.sources=secure_malloc(D.Nsources*sizeof(*D.sources));
    for (i=0; i<D.Nsources; ++i)
      fscanf(fd, "source%lf%lf%lf\n", &D.sources[i].x,
	     &D.sources[i].y, &D.sources[i].a);
    fclose(fd);
  }
  return D;
}

double intensity(diff_data_t D, double *I) {
  double dx=D.x2-D.x1, dy=D.y2-D.y1, dist2=D.dist*D.dist, 
    x, y, d, phi, I_re, I_im, I_max=0.0;
  int i, j, k;
  for (j=0; j<D.Ny; ++j) { /* Iteration über Zeilen */
    y=D.y1+(j+0.5)*dy/D.Ny;
    for (i=0; i<D.Nx; ++i) {  /* Iteration über Spalten */
      x=D.x1+(i+0.5)*dx/D.Nx;
      I_re=I_im=0.0;
      for (k=0; k<D.Nsources; ++k) {  /* Iteration über Quellen */
	d=sqrt((x-D.sources[k].x)*(x-D.sources[k].x)+
	       (y-D.sources[k].y)*(y-D.sources[k].y)+dist2);
	phi=6.28318530717958648*d/D.lambda;
	I_re+=D.sources[k].a*cos(phi)/d;
	I_im+=D.sources[k].a*sin(phi)/d;
      }
      I[i+D.Nx*j]=I_re*I_re+I_im*I_im;  /* Absolutquadrat = Intensität */
      if (I[i+D.Nx*j]>I_max)
	I_max=I[i+D.Nx*j];
    }  
  }
  return I_max;
}

int main(int argc, char *argv[]) {
  int C_rank, C_size, *Ny_l, *Ny0_l, i;
  double *I_l, *I=NULL, I_max_l, I_max;
  diff_data_t D={0}, D_l={0};
  FILE *out;
  MPI_Datatype row_t;
  /* MPI initialisieren */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &C_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &C_size);
  if (argc>=2)
    D=read_input(argv[1]);  /* Daten einlesen */
  if (D.Nsources==0)  /* keine Eingabedatei */
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  /* berechne Breite des Streifens für jeden Prozess */
  Ny_l=secure_malloc(C_size*sizeof(*Ny_l));
  Ny0_l=secure_malloc(C_size*sizeof(*Ny0_l));
  for (i=0; i<C_size; ++i) {
    Ny_l[i]=(i+1)*D.Ny/C_size-i*D.Ny/C_size;
    Ny0_l[i]=i*D.Ny/C_size;
  }
  /* rechne globale Geometriedaten auf lokale Daten um */
  D_l=D;
  D_l.Ny=Ny_l[C_rank];
  D_l.y1=Ny0_l[C_rank]*(D.y2-D.y1)/D.Ny+D.y1;
  D_l.y2=(Ny0_l[C_rank]+Ny_l[C_rank])*(D.y2-D.y1)/D.Ny+D.y1;
  I_l=secure_malloc(D.Nx*D_l.Ny*sizeof(*I_l));
  I_max_l=intensity(D_l, I_l);  /* berechne Intensität */
  /* ermittle maximale Intensität über alle Prozesse */
  MPI_Allreduce(&I_max_l, &I_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  for (i=0; i<D.Nx*D_l.Ny; ++i)  /* Intensitäten auf [0, 255] normieren */
    I_l[i]=255.0-floor(I_l[i]/I_max*255.0+0.5);
  /* Daten einsammeln und ausgeben */
  if (C_rank==0)
    I=secure_malloc(D.Nx*D.Nx*sizeof(*I));
  MPI_Type_contiguous(D.Nx, MPI_DOUBLE, &row_t);  /* Datentyp für Zeile */
  MPI_Type_commit(&row_t);
  MPI_Gatherv(I_l, Ny_l[C_rank], row_t, I, Ny_l, Ny0_l, row_t,
              0, MPI_COMM_WORLD);
  if (C_rank==0 && argc>=3) {
    out=fopen(argv[2], "w");
    fprintf(out, "P5\n%i %i\n255\n", D.Nx, D.Ny);  /* PGM-Header */
    for (i=0; i<D.Nx*D.Ny; ++i)  /* Graustufen */
      fputc((int)I[i], out);
  }
  /* Ressourcen freigeben */
  free(I);     free(I_l);
  free(Ny_l);  free(Ny0_l);
  free(D.sources);
  MPI_Type_free(&row_t);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
