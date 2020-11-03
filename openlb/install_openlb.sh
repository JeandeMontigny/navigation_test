#!/bin/bash

BDM_SCRIPT_DIR=$(readlink -e $(dirname "${BASH_SOURCE[0]}"))
cd $BDM_SCRIPT_DIR

git clone https://github.com/openLB/openLB
cd openLB

git apply ../add_fPIC_flag.patch

make -j$(grep -c ^processor /proc/cpuinfo)
