#!/bin/bash

git clone https://github.com/openLB/openLB
cd openLB

git apply ../add_fPIC_flag.patch

make
