/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


void string(double *u, double *u_old, double *u_new, int N, double eps) {
  int i;
  u_new[0]=u[0];
  for (i=1; i<N; ++i)
    u_new[i]=eps*(u[i-1]+u[i+1])+2.0*(1.0-eps)*u[i]-u_old[i];
  u_new[N]=u[N];
}
