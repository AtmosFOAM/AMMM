#!/bin/bash -ve

# Copy old case and compare differences
rm -rf constant [0-9]*
mkdir -p constant/cMesh 0/polyMesh 0/cMesh
cp -r ../constant/polyMesh constant/cMesh
cp -r ../constant/rMesh/polyMesh constant
cp ../0/rMesh/polyMesh/points 0/polyMesh
cp ../0/mmPhi 0/cMesh/meshPot
cp ../0/rMesh/p ../0/rMesh/U 0
ln -sf ../system/dynamicMeshDict constant/dynamicMeshDict
ln -sf ../system/transportProperties constant/transportProperties
ln -sf ../system/turbulenceProperties constant/turbulenceProperties

# Solve the incompressible Euler
pimpleOTFoam >& log &
tail -f log

# Compare with a saved solution
for time in 100000; do
    for var in p U monitor; do
        sumFields $time ${var}Diff $time $var ../$time/rMesh $var -scale1 -1
        echo That was differences for $var at time $time. 
        read -p "Press return to continue"
    done
    #diff $time/polyMesh/points ../$time/rMesh/polyMesh/points
done

time=200
var=monitor
sumFields $time ${var}Diff $time $var ../$time/rMesh $var -scale1 -1

sumFields $time meshPotDiff $time/cMesh meshPot ../$time mmPhi -scale1 -1

for var in detHess laplacianAPhi source gradPhif ; do
    sumFields $time ${var}Diff $time/cMesh $var ../$time $var -scale1 -1
    echo That was differences for $var at time $time. 
    read -p "Press return to continue"
done

# Calculate and plot the vorticity
time=100000
postProcess -func vorticity -time $time
writeuvw -time $time vorticity
gmtFoam -time $time vorticity
evince $time/vorticity.pdf &

