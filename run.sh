#!/bin/bash


# set OpenLB
BDM_SCRIPT_DIR=$(readlink -e $(dirname "${BASH_SOURCE[0]}"))
cd $BDM_SCRIPT_DIR
source $BDM_SCRIPT_DIR/../../util/main.sh

cd openlb

# wget https://www.openlb.net/wp-content/uploads/2020/11/olb-1.4r0.tgz
# tar -xzf olb-1.4r0.tgz
# rm olb-1.4r0.tgz
cd olb-1.4r0
# cp ../config.mk .

make

# Quick fix for linking error inside docker container:
sudo mv /usr/lib/x86_64-linux-gnu/libz.a /usr/lib/x86_64-linux-gnu/libz.a.bak

# set BioDynaMo
# chose if perform clean bdm build
# bdm version: aa8e3048417ffd2a0177cd7639a989df304767b2
export BPE_NO_CLEANBUILD=1

source $BDM_SCRIPT_DIR/../../util/default-compile-script.sh "-Djemalloc=off" "-Djemalloc=off"

export OMP_NUM_THREADS=6
# export OMP_PROC_BIND=close
export OMP_PROC_BIND=true
export OMP_PLACES=cores
export BPE_REPEAT=1

./openlb_sim

# export paraview visualisation
# export SIM_RESULT_PREFIX="$BDM_SCRIPT_DIR/build/output"
#
# render \
#   "$SIM_RESULT_PREFIX" \
#   "$BDM_SCRIPT_DIR/../../visualization/epidemio_bus.py"
