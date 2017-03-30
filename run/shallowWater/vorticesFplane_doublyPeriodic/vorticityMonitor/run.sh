#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Createt initial conditions on the rMesh and plot
time=0
cp -r init_0 0
# Create Gaussian patches of voriticty
setGaussians initDict -region rMesh
# Invert to find the wind field
invertVorticity -time $time initDict -region rMesh

# Calculate the height in balance and plot
setBalancedHeight -region rMesh

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 5 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    movingshallowWaterFoamH -reMeshOnly

    # Re-create the initial conditions and re-plot
    setGaussians initDict -region rMesh
    # Invert to find the wind field
    invertVorticity -time $time initDict -region rMesh

    let meshIter+=1
done
# Calculate the height in balance and plot
setBalancedHeight -region rMesh
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

postProcess -func rMesh/vorticity2D -region rMesh -time $time
rm $time/rMesh/vorticity
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

gmtFoam -time $time monitor -region rMesh
evince $time/monitor.pdf &

# Solve the SWE
movingshallowWaterFoamH >& log & sleep 0.01; tail -f log

# Post process
time=700000
gmtFoam -time $time hU -region rMesh
evince $time/hU.pdf &

postProcess -func rMesh/vorticity2D -time $time -region rMesh
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=200000
postProcess -func rMesh/vorticity2D -region rMesh -time $time':'
gmtFoam vorticity -region rMesh -time $time':'


# Animation of vorticity
postProcess -func rMesh/vorticity2D -region rMesh
gmtFoam vorticity -region rMesh
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf

