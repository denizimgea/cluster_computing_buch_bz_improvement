/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

typedef struct { double phi, theta; } point;

void error(char *format, ...) {  /* Fehlerbehandlung */
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

double potential(int n, point *r) {  /* elektrisches Potentials */
  int i, j;
  double E=0, x1, y1, z1, x2, y2, z2, dx, dy, dz, dr;
  for (i=0; i<n; ++i) {
    x1=cos(r[i].theta)*sin(r[i].phi);
    y1=sin(r[i].theta)*sin(r[i].phi);
    z1=cos(r[i].phi);
    for (j=i+1; j<n; ++j) {
      x2=cos(r[j].theta)*sin(r[j].phi);
      y2=sin(r[j].theta)*sin(r[j].phi);
      z2=cos(r[j].phi);
      dx=x1-x2;  dy=y1-y2;  dz=z1-z2;  dr=sqrt(dx*dx+dy*dy+dz*dz);
      if (dr>n*2.0*DBL_MIN) E+=1.0/dr; else return DBL_MAX;
    }
  }
  return E;
}

int main(int argc, char *argv[]) {
  int i, n;	/* Zähler und Zahl der Ladungen */
  point *r;	/* n-dimensionaler Vektor */
  FILE* fd;	/* Filedescriptor */
  if (argc<3) 
    error("Verwendung: %s <ein> <aus>\n", argv[0]);
  if ((fd=fopen(argv[1], "r"))==NULL) 
    error("Kann Eingabedatei %s nicht öffnen.\n", argv[1]);
  if ((fscanf(fd, "%d", &n))!=1)
    error("Fehler beim Lesen der Koordiantenanzahl.\n");
  n/=2;  /* Zahl der Parameter = 2 * Zahl der Ladungen */
  if ((r=malloc(n*sizeof(*r)))==NULL)
    error("Speicher voll.\n");
  for (i=0; i<n; ++i)  /* Koordinaten einlesen */
    if ((fscanf(fd, "%le%le", &r[i].phi, &r[i].theta))!=2) 
      error("Fehler beim Lesen von r[%d] aus %s.\n", i, argv[1]);
  fclose(fd); 
  if ((fd=fopen(argv[2], "w"))==NULL) 
    error("Kann Ausgabedatei %s nicht öffnen.\n", argv[2]);
  fprintf(fd, "%e\n", potential(n, r));  
  fclose(fd);  
  free(r);  /* Speicher freigeben */
  return EXIT_SUCCESS;
}
