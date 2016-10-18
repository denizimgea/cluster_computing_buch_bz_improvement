/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


/* berechnet Integral über f von a bis b mit Mittelpunktsregel an n 
   Stützstellen. Liefert die k-te Partialsumme (0<=k<tasks) zurück. */
double pintegral(double (*f)(double), double a, double b, 
		 long n, int k, int tasks) {
  double result=0.0;
  double h=(b-a)/n;
  long l;
  a=a+h/2;
  for (l=k; l<n; l+=tasks) 
    result+=f(a+l*h);
  return h*result;
}
