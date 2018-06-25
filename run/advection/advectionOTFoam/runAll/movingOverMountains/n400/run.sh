#!/bin/bash -e

# Clear the case
foamListTimes -withZero -rm
rm -rf constant/cMesh constant/polyMesh constant/T_init legends gmt.history

# Create initial mesh
blockMesh
mkdir constant/cMesh
cp -r constant/polyMesh constant/cMesh

# Create initial conditions
cp -r init0 0
# Set tracer
cp 0/T constant/T_init
setAnalyticTracerField -velocityDict advectionDict \
                       -tracerDict tracerDict -name T
# Set divergence-free velocity field
setVelocityField -dict advectionDict

# Iterate, creating a mesh adapted to the initial conditions on that mesh
terrainFollowingMesh
sed 's/MAXMESHVELOCITY/0/g' system/OTmeshDictTemplate | \
    sed 's/MESHRELAX/0.5/g' > system/OTmeshDict
meshIter=0
until [ $meshIter -ge 20 ]; do
    echo Mesh generation iteration $meshIter

    advectionOTFoam -reMeshOnly
    setAnalyticTracerField -velocityDict advectionDict \
                           -tracerDict tracerDict -name T

    let meshIter+=1
done

# Run
sed 's/MAXMESHVELOCITY/1e6/g' system/OTmeshDictTemplate | \
    sed 's/MESHRELAX/0/g' > system/OTmeshDict
advectionOTFoam >& log &
