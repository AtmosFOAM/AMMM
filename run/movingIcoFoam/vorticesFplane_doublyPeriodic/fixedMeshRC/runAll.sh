#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant system core log legends gmt.history

# Copy from other case
oldCase=../fixedMesh
mkdir -p 0 constant system
cp $oldCase/system/* system
cp $oldCase/system/rMesh/* system
cp -r $oldCase/constant/rMesh/polyMesh/ constant
cp $oldCase/0/rMesh/polyMesh/points constant/polyMesh
cp -r $oldCase/constant/gmtDicts/ constant
cp $oldCase/0/rMesh/* 0
rmbakall
rm 0/twoOmegaf 0/Uf 0/vorticity 0/h 0/meshPhi 0/monitor 0/streamFunction \
   0/two_dxOmega
mv 0/u 0/U
oldDir=/home/hilary/OpenFOAM/OpenFOAM-dev/tutorials/incompressible/icoFoam/cavity/cavity
cp $oldDir/constant/transportProperties constant
meld $oldDir/system system

# Solve the incompressibe Euler equations
icoFoam >& log & sleep 0.01; tail -f log

# Post process
time=700000
postProcess -func vorticity -time $time
writeuvw -time $time vorticity
gmtFoam -time $time vorticity
evince $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=200000
postProcess -func rMesh/vorticity2D -region rMesh -time $time':'
gmtFoam vorticity -region rMesh -time $time':'


# Animation of vorticity
postProcess -func rMesh/vorticity2D -region rMesh
gmtFoam vorticity -region rMesh
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf

