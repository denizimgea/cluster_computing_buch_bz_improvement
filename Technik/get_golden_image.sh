#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              



###  Konfigurationsdaten lesen  ########################################
. /etc/cluster/cluster.conf

###  Verzeichnisse anlegen  ############################################
mkdir -p $NODES_DIR
mkdir -p $NODES_DIR/golden_image

###  Dateien vom Golden-Node kopieren  #################################
rsync --rsh=ssh -auz --delete \
    --include=/bin   --include=/etc  --include=/lib \
    --include=/root  --include=/sbin --include=/usr \
    --include=/var \
    --exclude="/*/" --exclude="/vmlinuz*" --exclude=/lib/modules \
    $GOLDEN_NODE_IP:/ $NODES_DIR/golden_image

###  Dateien zum Setzen des Hostnamen entfernen  #######################
rm -rf $NODES_DIR/golden_image/etc/init.d/hostname.sh 
rm -rf $NODES_DIR/golden_image/etc/rcS.d/S40hostname.sh
rm -rf $NODES_DIR/golden_image/etc/hostname

###  Konfigurationsdatei der Netzwerkinterfaces schreiben  #############
cat <<EOF > $NODES_DIR/golden_image/etc/network/interfaces
auto lo
iface lo inet loopback
EOF
