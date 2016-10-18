#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



# Programmnamen extrahieren
PNAME=`basename $1`
# Kommando mit allen weiteren Argumenten ausführen und 
# Standardfehlerausgabe und Standardausgabe für jeden Rang umleiten
"$@" > $PNAME.stdout.$LAMRANK 2> $PNAME.stderr.$LAMRANK
# Return-Code weiterreichen
exit $? 
