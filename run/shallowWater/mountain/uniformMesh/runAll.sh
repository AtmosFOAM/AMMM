#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/cMesh
cp -r constant/polyMesh constant/cMesh
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict

# Create the mountain
cp init_0/h0_init constant
setAnalyticTracerField -time 0 -name h0 -tracerDict mountainDict
mkdir 0
mv constant/h0 0
rm constant/h0*
gmtFoam h0Mesh
gv 0/h0Mesh.pdf &

## Create the sponge
#createSpongeLayer
#cp constant/muSponge 0

# Create diffusion coefficient
cp init_0/diffusionCoeff_init constant
setAnalyticTracerField -time 0 -name diffusionCoeff \
     -tracerDict diffusionDict

# Create initial conditions
cp -r init_0/* 0
time=0
setBalancedHeightRC
gmtFoam -time 0 hMesh
gv 0/hMesh.pdf &

# Solve the SWE
shallowWaterOTFoam -fixedMesh >& log & sleep 0.01; tail -f log

# Difference between solutions
time=10000
sumFields $time UDiff $time U 0 U -scale1 -1
sumFields $time hDiff  $time h  0 h  -scale1 -1
gmtFoam hUDiff -time $time
gv $time/hUDiff.pdf &

for time in [0-9]*; do
    sumFields $time UDiff $time U 0 U -scale1 -1
    sumFields $time hDiff  $time h  0 h  -scale1 -1
done
gmtFoam hUDiff
eps2gif hUDiff.gif 0/hUDiff.pdf ?????/hUDiff.pdf ??????/hUDiff.pdf

