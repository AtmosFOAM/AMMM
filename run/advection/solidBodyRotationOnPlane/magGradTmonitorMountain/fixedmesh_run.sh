#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh constant/rMesh constant/pMesh

# Generate the mesh - the rMesh and pMesh are periodic but the 
# mesh is not periodic
blockMesh
blockMesh -region rMesh
blockMesh -region pMesh

# Calculate the initial conditions before imposing the terrain
cp -r init0 0
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

# Raise the mountain
terrainFollowingMesh -region pMesh

postProcess -func writeCellVolumes -time 0 -region pMesh

gmtFoam -time 0 -region pMesh UTmesh
#evince 0/UTmesh.pdf &
gmtFoam -time 0 -region pMesh UT
gmtFoam -time 0 -region pMesh monitor
#evince 0/monitor.pdf &

# Run
movingScalarTransportFoam -fixedMesh >& log & sleep 0.001; tail -f log
