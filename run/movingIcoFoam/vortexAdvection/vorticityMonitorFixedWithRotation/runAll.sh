#!/bin/bash -e
#
set -o pipefail

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh core log legends gmt.history

cp -r init_0 0

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Createt initial initial conditions on the rMesh
setGaussians initDict -region rMesh
invertVorticity -time 0 initDict -region rMesh
gmtFoam -time 0 vorticity -region rMesh
evince 0/vorticity.pdf &

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 10 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    movingIcoFoamH -reMeshOnly

    # Re-create the initial conditions and re-plot
    setGaussians initDict -region rMesh
    invertVorticity -time 0 initDict -region rMesh
    gmtFoam -time 0 vorticity -region rMesh
    
    let meshIter+=1
done

cp 0/meshPhi 0/rMesh

# Solve incompressible Euler equations on a moving mesh
movingIcoFoamH -fixedMesh >& log & sleep 0.01; tail -f log
#movingIcoFoamH |&  tee log

# What to plot and when
# Approximate time for one complete revolution
time=3150
field=vorticity
postProcess -func rMesh/vorticity2D -time $time -region rMesh

# Plot field $field at time $time
gmtFoam -time $time $field -region rMesh
gv $time/$field.pdf &

# Plots for all time
gmtFoam $field -region rMesh
#pdfjam --outfile $field.pdf --landscape 0/$field.pdf ???/$field.pdf ????/$field.pdf
eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf ????/$field.pdf

