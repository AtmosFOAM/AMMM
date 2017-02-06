#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh constant/rMesh

# Generate the mesh
blockMesh
mkdir constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Calculate the initial conditions
cp -r init0 0
setVelocityField -region rMesh -dict advectionDict
setAnalyticTracerField -region rMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region rMesh UT
gv 0/UT.pdf &

# Run
movingScalarTransportFoam >& log & sleep 0.001; tail -f log

# Plot results
for time in [1-9]*; do
    ln -s ../../0/rMesh/Uf $time/rMesh/Uf
    ln -s ../../0/rMesh/U $time/rMesh/U
done

gmtFoam UT -region rMesh
eps2gif UT.gif 0/UT.pdf ??/UT.pdf ???/UT.pdf

