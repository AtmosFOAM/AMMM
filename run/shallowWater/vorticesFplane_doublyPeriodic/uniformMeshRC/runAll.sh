#!/bin/bash -e
#

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/cMesh
cp -r constant/polyMesh constant/cMesh
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict

# Createt initial conditions
rm -rf 0
cp -r init_0 0
time=0
# Create Gaussian patches of voriticty
setGaussians initDict
# Invert to find the wind field
invertVorticity -time $time initDict
gmtFoam -time $time vorticity
gv $time/vorticity.pdf &

# Calculate the height in balance and plot
setBalancedHeightRC
gmtFoam -time $time hU
gv $time/hU.pdf &

# Solve the SWE
shallowWaterOTFoam >& log & sleep 0.01; tail -f log
#shallowWaterFoam >& log & sleep 0.01; tail -f log

# Post process
time=700000
gmtFoam -time $time hU
gv $time/hU.pdf &

postProcess -func vorticity -time $time
writeuvw vorticity -time $time
mv $time/vorticityz $time/vorticity
rm $time/vorticity?
gmtFoam -time $time vorticity
gv $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=200000
postProcess -func rMesh/vorticity2D -region rMesh -time $time':'
gmtFoam vorticity -region rMesh -time $time':'


# Animation of vorticity
postProcess -func rMesh/vorticity2D -region rMesh
gmtFoam vorticity -region rMesh
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf


# Debugging
time=1000
for var in phi; do
    sumFields $time ${var}Diff $time ${var} 0 ${var} -scale1 -1
    echo Time $time variable $var
    read -p "Press enter to continue"
done

