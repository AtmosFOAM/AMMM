#!/bin/bash -e
# Create the case, run and post-process

## Clear the case
foamListTimes -withZero -rm
rm -rf constant/cMesh constant/polyMesh legends gmt.history

## Create initial mesh
blockMesh
mkdir constant/cMesh
cp -r constant/polyMesh constant/cMesh

## Create initial conditions
cp -r init_0 0
# set tracer
setVelocityField -dict advectionDict
setAnalyticTracerField -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

# Raise the mountain
terrainFollowingMesh

## Solve the SWE
advectionOTFoam -fixedMesh >& log & sleep 0.01; tail -f log
