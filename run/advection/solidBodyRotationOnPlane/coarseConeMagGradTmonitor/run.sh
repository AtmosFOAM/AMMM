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

# Plot results
time=0
field=mesh
field=monitor
field=T
field=UT
gmtFoam $field -region pMesh -time $time':'
eps2gif $field.gif 0/$field.pdf ??/$field.pdf ???/$field.pdf

# Conservation of T
globalSum T -region pMesh

# Make links for animategraphics
field=T
field=mesh
field=monitor
mkdir -p animategraphics
ln -s ../0/UT.pdf animategraphics/T_0.pdf
for time in [1-9]*; do
    let t=$time/50
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

# Plot the mesh every time-step
gmtFoam -region pMesh Tmesh
eps2gif Tmesh.gif ?/Tmesh.pdf ??/Tmesh.pdf ???/Tmesh.pdf

