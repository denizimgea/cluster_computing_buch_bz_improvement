#!/bin/bash

# Beispiel-Programm aus dem Buch  
# »Cluster Computing« von         
# Heiko Bauke und Stephan Mertens 
#                                 
# Springer 2005                   
# ISBN 3-540-42299-4              


#
###  NFS-Root-Verzeichnisse anlegen  ###################################

. /etc/cluster/cluster.conf  #  Konfigurationsdaten lesen
NODE_NR=0
for MAC in `cat $MAC_FILE`; do
    (( NODE_NR+=1 ))
    NODE_NAME=$NODE_BASE_NAME$NODE_NR
    NODE_IP=$NETWORK_ADDRESS.$NODE_NR
    NFS_ROOT_DIR=$NODES_DIR/$NODE_NAME
    echo "Bulding NFS-Root for node: $NODE_NAME"
    # Verzeichnisse kopieren
    mkdir -p $NFS_ROOT_DIR
    rsync -auz --delete \
	$NODES_DIR/golden_image/bin  $NODES_DIR/golden_image/etc \
	$NODES_DIR/golden_image/lib  $NODES_DIR/golden_image/root \
	$NODES_DIR/golden_image/sbin $NODES_DIR/golden_image/var \
	$NFS_ROOT_DIR
    # weitere Verzeichnisse anlegen
    mkdir -p $NFS_ROOT_DIR/proc
    mkdir -p $NFS_ROOT_DIR/home
    mkdir -p $NFS_ROOT_DIR/tmp
    mkdir -p $NFS_ROOT_DIR/usr
    mkdir -p $NFS_ROOT_DIR/dev
    ( cd $NFS_ROOT_DIR/dev; MAKEDEV std console )
    # /etc/fstab anlegen
    cat << EOF > $NFS_ROOT_DIR/etc/fstab
$SERVER_IP:$NFS_ROOT_DIR / nfs defaults	  0 0
tmpfs    /tmp   tmpfs  defaults,size=64m  0 0
proc	 /proc  proc   defaults		  0 0
$SERVER_IP:$NODES_DIR/golden_image/usr /usr nfs defaults 0 0
$SERVER_IP:/usr/local /usr/local nfs defaults 0 0
$SERVER_IP:/home      /home      nfs defaults 0 0
EOF
    # Konfigurationsdatei mit Bootparameter 
    # in /tftpboot/pxelinux.cfg anlegen
    IP2NUM='(((\1*256+\2)*256+\3)*256)+\4'
    NODE_IP_HEX=`echo $NODE_IP | \
        sed -e "s/\([0-9]*\).\([0-9]*\).\([0-9]*\).\([0-9]*\)/$IP2NUM/" |\
        bc`
    NODE_IP_HEX=`printf "%008X" $NODE_IP_HEX`
    mkdir -p /tftpboot/pxelinux.cfg
    cat <<EOF > /tftpboot/pxelinux.cfg/$NODE_IP_HEX
DEFAULT $KERNEL_IMAGE
APPEND root=/dev/nfs \
nfsroot=$SERVER_IP:$NFS_ROOT_DIR,rsize=8192,rsize=8129,rw ip=dhcp 
EOF
done
