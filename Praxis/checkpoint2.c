/* Beispiel-Programm aus dem Buch  */
/* »Cluster Computing« von         */
/* Heiko Bauke und Stephan Mertens */
/*                                 */
/* Springer 2005                   */
/* ISBN 3-540-42299-4              */


#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <ckpt.h>

void logging(void *str) {
  fprintf(stderr, "%s", str);
}

int main(int argc, char *argv[]) {
  int i, j, i_max=1;
  double sum, x;
  FILE *out;
  const char *file_name="checkpoint2.log";
  struct ckptconfig conf;  
  /* Call-Back-Funktionen registrieren */
  ckpt_on_preckpt(logging, "sichern ...\t");
  ckpt_on_postckpt(logging, "und nun weiter ...\n");
  /* Ckpt-Parameter setzen */
  conf.asyncsig=SIGTERM;  /* ansynchrones Checkpointing durch SIGTERM */
  conf.continues=1;       /* nach Checkpoint weitermachen */
  conf.msperiod=0;        /* kein periodisches Checkpointing */
  conf.flags=CKPT_ASYNCSIG | CKPT_CONTINUE | CKPT_MSPERIOD;
  ckpt_config(&conf, NULL);
  
  if (argc>1)
    i_max=atoi(argv[1]);
  remove(file_name);
  for (i=0; i<i_max; ++i) {
    sum=0.0;
    for (j=0; j<10000000; ++j)
      sum+=sin((double)rand()/(double)RAND_MAX*3.141592654);
    out=fopen(file_name, "w+");  /* Datei öffnen und ...*/
    fprintf(out, "i = %i\tsum = %f\n", i, sum);
    fclose(out); /* .. gleich wieder schließen */
    ckpt_ckpt(NULL);  /* Checkpoint */
  }
  return EXIT_SUCCESS;
}
