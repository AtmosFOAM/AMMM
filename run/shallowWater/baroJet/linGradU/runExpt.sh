#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant core log* legends gmt.history *.dat

# Start from a previous solution
time=1000000
oldCase=../uniformMesh
cp -r $oldCase/$time $oldCase/constant .

# Calculate the rMesh based on the monitor function derived from Uf
movingshallowWaterFoamH -reMeshOnly

gmtFoam -time $time -region rMesh monitor
gv $time/monitor.pdf &

