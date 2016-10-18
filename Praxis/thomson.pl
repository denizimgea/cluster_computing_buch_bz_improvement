#!/usr/bin/perl -w

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



if (defined($ARGV[0])) { $N=$ARGV[0]; } else { $N=5; }
$pi=3.141592654;
print "@ \"Bounds\"\n";
print "\"Upper\" vector ".2*$N." ";
for ($i=0; $i<$N; ++$i) { print $pi." ".2*$pi." "; }
print "\n";
print "\"Lower\" vector ".2*$N." ";
for ($i=0; $i<2*$N; ++$i) { print "0 "; }
print "\n";
print "@@\n";
print "@ \"Evaluator\"\n";
print "\"Executable Name\" string \"./electrons\"\n";
print "\"Input Prefix\" string \"electrons_in\"\n";
print "\"Output Prefix\" string \"electrons_out\"\n";
print "@@\n"; 
print "@ \"Solver\"\n";
print "\"Debug\" int 1\n";
print "\"Initial X\" vector ".2*$N." ";
for ($i=0; $i<$N; ++$i) { print $pi*rand()." ".rand()." "; }
print "\n";
print "@@\n";
