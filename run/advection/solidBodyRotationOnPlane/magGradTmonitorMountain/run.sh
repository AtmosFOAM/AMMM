#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh constant/rMesh constant/pMesh

# Generate the mesh - no meshes are periodic
# mesh is not periodic
blockMesh
mkdir constant/rMesh constant/pMesh
cp -r constant/polyMesh constant/rMesh
cp -r constant/polyMesh constant/pMesh

# Calculate the initial conditions before imposing the terrain
cp -r init0 0
cp 0/pMesh/T constant/pMesh/T_init
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region pMesh UTmesh
gv 0/UTmesh.pdf &

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 10 ]; do
    echo Mesh generation iteration $meshIter

    # Calculate the rMesh based on the monitor function derived from Uf
    movingScalarTransportFoam -reMeshOnly

    # Re-create the initial conditions
    setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                           -tracerDict tracerDict -name T

    let meshIter+=1
done
# Re-create velocity field and re-plot
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region pMesh UTmesh
gv 0/UTmesh.pdf &
gmtFoam -time 0 -region pMesh monitor
evince 0/monitor.pdf &

# Raise the mountain
terrainFollowingMesh -region pMesh

# Run
movingScalarTransportFoam -colinParameter >& log & sleep 0.001; tail -f log

