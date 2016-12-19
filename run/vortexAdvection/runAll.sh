#!/bin/bash -e
#
./setup.sh
movingIcoFoamA >& log &
tail -f log

time=6
gmtFoam -time $time pU -region pMesh
gv $time/pU.pdf &

