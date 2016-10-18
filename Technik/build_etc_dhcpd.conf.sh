#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              


#
###  /etc/dhcpd.conf  anlegen  #########################################

. /etc/cluster/cluster.conf  #  Konfigurationsdaten lesen
echo Writing /etc/dhcpd.conf
(
    cat <<EOF
allow booting;
allow bootp;
use-host-decl-names on;

subnet $NETWORK_ADDRESS.0 netmask 255.255.255.0 {

  option domain-name "$DOMAIN";
  option broadcast-address $NETWORK_ADDRESS.255;
  option domain-name-servers $SERVER_IP;
  option routers $SERVER_IP;
  next-server $SERVER_IP;
  filename "pxelinux.0";

EOF
    NODE_NR=0
    for MAC in `cat $MAC_FILE`; do
	(( NODE_NR+=1 ))
	NODE_NAME=$NODE_BASE_NAME$NODE_NR
	NFS_ROOT_DIR=$NODES_DIR/$NODE_NAME
	NODE_IP=$NETWORK_ADDRESS.$NODE_NR
	cat <<EOF
  host $NODE_NAME {
    hardware ethernet $MAC;
    fixed-address $NODE_IP;
  }
EOF
    done
    cat <<EOF

}
EOF
) > /etc/dhcpd.conf
# DHCP-Server neu starten
/etc/init.d/dhcp restart > /dev/null
