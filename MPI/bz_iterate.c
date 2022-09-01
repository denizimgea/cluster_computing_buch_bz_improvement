/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


void bz_iterate(int *X, int *X_new, int matrix_width, int matrix_height, 
		int n, int k1, int k2, int g) {
  int i, j, k, a, b, s, x, x_n;
  const int dx[]={0, 1, 1, 1, 0,-1,-1,-1}, dy[]={1, 1, 0,-1,-1,-1, 0, 1};
  for (j=0; j<matrix_height; ++j)
    for (i=0; i<matrix_width; ++i) {
      x=X[i+(matrix_width+2)*j];
      a=b=0;
      s=x;
      for (k=0; k<8; ++k) {
	x_n=X[(i+dx[k])+(matrix_width+2)*(j+dy[k])];
	s+=x_n;
	if (0<x_n && x_n<n)  ++a;
	else if (x_n==n)     ++b;
      }
      if (x==0)             X_new[i+(matrix_width+2)*j]=a/k1+b/k2;
      else if (0<x && x<n)  X_new[i+(matrix_width+2)*j]=s/(a+b+1)+g;
      else                  X_new[i+(matrix_width+2)*j]=0;
      if (X_new[i+(matrix_width+2)*j]>n)
	X_new[i+(matrix_width+2)*j]=n;
    }
}
