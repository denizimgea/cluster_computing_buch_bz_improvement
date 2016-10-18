#!/bin/bash

# Beispiel-Programm aus dem Buch  
# �Cluster Computing� von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



# Programmnamen extrahieren
PNAME=`basename $1`
# Kommando mit allen weiteren Argumenten ausf�hren und 
# Standardfehlerausgabe und Standardausgabe f�r jeden Rang umleiten
"$@" > $PNAME.stdout.$LAMRANK 2> $PNAME.stderr.$LAMRANK
# Return-Code weiterreichen
exit $? 
