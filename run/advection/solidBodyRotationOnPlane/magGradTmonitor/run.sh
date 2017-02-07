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

gmtFoam -time 0 -region rMesh UTmesh
evince 0/UTmesh.pdf &

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 10 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    movingScalarTransportFoam -reMeshOnly

    # Re-create the initial conditions
    setAnalyticTracerField -region rMesh -velocityDict advectionDict \
                           -tracerDict tracerDict -name T
    
    let meshIter+=1
done
# Re-create velocity field and re-plot
setVelocityField -region rMesh -dict advectionDict
setAnalyticTracerField -region rMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region rMesh UTmesh
gmtFoam -time 0 -region rMesh monitor
evince 0/monitor.pdf &

# Run
movingScalarTransportFoam >& log & sleep 0.001; tail -f log

# Plot results
time=30
field=UTmesh
field=Tmonitor
gmtFoam $field -region rMesh -time $time':'
eps2gif $field.gif 0/$field.pdf ??/$field.pdf ???/$field.pdf

