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

# Raise the mountain
terrainFollowingMesh

# Draw initial condition
gmtFoam -time 0 UT
gv 0/UT.pdf &

# Run
advectionOTFoam -fixedMesh >& log & sleep 0.01; tail -f log

