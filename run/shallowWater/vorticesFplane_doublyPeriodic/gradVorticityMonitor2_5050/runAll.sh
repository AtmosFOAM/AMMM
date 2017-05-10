#!/bin/bash -e

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/cMesh
cp -r constant/polyMesh constant/cMesh
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict

# Create initial conditions
rm -rf [0-9]* core
cp -r init_0 0
time=0
# Create Gaussian patches of voriticty
setGaussians initDict
# Invert to find the wind field
invertVorticity -time $time initDict

#gmtFoam -time $time vorticityMesh
#ev $time/vorticityMesh.pdf

# Iterate, creating a mesh adapted to the initial conditions on that mesh
meshIter=0
until [ $meshIter -ge 20 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    shallowWaterOTFoam -reMeshOnly
    
    # Re-create the initial conditions and re-plot
    setGaussians initDict >& /dev/null
    # Invert to find the wind field
    invertVorticity -time $time initDict >& /dev/null

#    gmtFoam -time $time vorticityMesh >& /dev/null
    let meshIter+=1

    mkdir $meshIter
    cp -r 0/polyMesh $meshIter
done

gmtFoam mesh >& /dev/null
eps2gif mesh.gif ?/mesh.pdf ??/mesh.pdf
rm -r [1-9]*

# Calculate the height in balance and plot
setBalancedHeightRC
gmtFoam -time $time hUmesh
gv $time/hUmesh.pdf &

# Solve the SWE
shallowWaterOTFoam >& log & sleqep 0.01; tail -f log
#shallowWaterFoam >& log & sleep 0.01; tail -f log

# Post process
time=700000
gmtFoam -time $time hU
gv $time/hU.pdf &

time=100000
case=.
postProcess -func vorticity -time $time -case $case
writeuvw -time $time vorticity -case $case
mv $case/$time/vorticityz $case/$time/vorticity
rm $case/$time/vorticity?
gmtFoam -time $time vorticityMesh -case $case
gv $case/$time/vorticityMesh.pdf &


