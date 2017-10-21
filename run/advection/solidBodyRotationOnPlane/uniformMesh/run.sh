#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh constant/rMesh constant/pMesh

# Generate the mesh
blockMesh
mkdir constant/rMesh constant/pMesh
cp -r constant/polyMesh constant/rMesh
cp -r constant/polyMesh constant/pMesh

# Calculate the initial conditions
cp -r init0 0
cp 0/pMesh/T constant/pMesh/T_init
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region pMesh UT
evince 0/UT.pdf &

# Run
movingScalarTransportFoam -fixedMesh >& log & sleep 0.001; tail -f log
gmtPlot plots/plotMass.gmt
