#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



TITLE="Rang $LAMRANK auf $HOSTNAME";
if [ $1 = --only ]; then 
  # nur ausgesuchte Ränge in xterm starten
  if echo $2 | grep -Eq '(^|,)'$LAMRANK'($|,)'; then
    shift 2
    xterm -T "$TITLE" -e "$@"
  else
    shift 2
    "$@"
  fi
else
  # alle Ränge in xterm starten
  xterm -T "$TITLE" -e "$@"
fi
exit 0
