#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



# Kommandozeilenargumente testen
if [ $# -ne 1 -o "$1" != "--add" -a "$1" != "--remove" ]; then 
  echo $0: falsche Parameter oder Parameterzahl >&2
else  
  # LAM/MPI Hosts ermitteln
  HOSTS=`lamnodes -n -c 2> /dev/null`
  # Hosts Zugriff auf lokalen X-Server erlauben oder verbieten 
  for i in $HOSTS; do
    if [ $1 = "--add" ]; then xhost +$i; else xhost -$i; fi
  done
fi
