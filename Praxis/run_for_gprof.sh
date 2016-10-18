#!/bin/bash

# Beispiel-Programm aus dem Buch  
# �Cluster Computing� von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



# Programmnamen extrahieren
PNAME=`basename $1`
# Dateinamen f�r Profiling-Datei setzen
export GMON_OUT_PREFIX="gmon.$PNAME.$LAMRANK"
# Kommando mit allen weiteren Argumenten ausf�hren
"$@" 
# Return-Code weiterreichen
exit $? 
