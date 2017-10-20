#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh constant/rMesh constant/pMesh

# Generate the mesh - the rMesh and pMesh are periodic but the 
# mesh is not periodic
blockMesh
blockMesh -region rMesh
blockMesh -region pMesh

# Calculate the initial conditions
cp -r init0 0
cp 0/pMesh/T constant/pMesh/T_init
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region pMesh UTmesh
evince 0/UTmesh.pdf &

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
gmtFoam -time 0 -region pMesh UT
gmtFoam -time 0 -region pMesh monitor
evince 0/monitor.pdf &

# Run
movingScalarTransportFoam >& log & sleep 0.001; tail -f log

# De-bugging plots
gmtFoam -time 0.5 -region pMesh volError
gv 0.5/volError.pdf &

# Check conservation of T
globalSum -region pMesh T
more globalSumpMeshT.dat
# Check conservation and unity of uniT
globalSum -region pMesh uniT
more globalSumpMeshuniT.dat

