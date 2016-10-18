#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              


#
###  /etc/exports anlegen  #############################################

. /etc/cluster/cluster.conf  #  Konfigurationsdaten lesen
echo Writing /etc/exports
( 
    cat <<EOF
/usr/local  $NETWORK_ADDRESS.0/255.255.255.0(no_root_squash,ro)
/home       $NETWORK_ADDRESS.0/255.255.255.0(rw)
$NODES_DIR/golden_image/usr  \
$NETWORK_ADDRESS.0/255.255.255.0(no_root_squash,ro)
EOF
    NODE_NR=0
    for MAC in `cat $MAC_FILE`; do
	(( NODE_NR+=1 ))
	NODE_NAME=$NODE_BASE_NAME$NODE_NR
	NFS_ROOT_DIR=$NODES_DIR/$NODE_NAME
	NODE_IP=$NETWORK_ADDRESS.$NODE_NR
	echo "$NFS_ROOT_DIR  $NODE_IP(no_root_squash,rw)"
    done
) > /etc/exports
# NFS-Server neu starten
/etc/init.d/nfs-kernel-server restart > /dev/null  
