#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              


#
###   /etc/hosts anlegen  ##############################################

. /etc/cluster/cluster.conf  #  Konfigurationsdaten lesen
echo Writing /etc/hosts
( 
    cat <<EOF
127.0.0.1 localhost
$SERVER_IP $SERVER.$DOMAIN $SERVER
$GOLDEN_NODE_IP $GOLDEN_NODE.$DOMAIN $GOLDEN_NODE
EOF
    NODE_NR=0
    for MAC in `cat $MAC_FILE`; do
	(( NODE_NR+=1 ))
	NODE_NAME=$NODE_BASE_NAME$NODE_NR
	NODE_IP=$NETWORK_ADDRESS.$NODE_NR
	echo $NODE_IP $NODE_NAME.$DOMAIN $NODE_NAME
    done
) > /etc/hosts
NODE_NR=0
for MAC in `cat $MAC_FILE`; do
    (( NODE_NR+=1 ))
    NODE_NAME=$NODE_BASE_NAME$NODE_NR
    NFS_ROOT_DIR=$NODES_DIR/$NODE_NAME
    cp /etc/hosts $NFS_ROOT_DIR/etc/hosts
done
