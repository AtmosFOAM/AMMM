#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/polyMesh constant/*/polyMesh core log* legends gmt.history *.dat

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Createt initial conditions on the rMesh and plot h and the mesh
time=0
cp -r init0 0
setPlaneJet -region rMesh initDict

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 5 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    movingshallowWaterFoamH -reMeshOnly

    # Re-create the initial conditions
    setPlaneJet -region rMesh initDict

    let meshIter+=1
done

gmtFoam -time $time -region rMesh hMesh
evince $time/hMesh.pdf &

# Solve the shallow water equations
movingshallowWaterFoamH >& log & sleep 0.01; tail -f log

# Plot animation of solutions
field=vorticity
postProcess -func rMesh/vorticity2D -region rMesh
field=mesh
gmtFoam $field -region rMesh
#eps2gif hU.gif 0/hU.pdf [1-9]00000/hU.pdf 1e+06/hU.pdf &

# links for latex animations
mkdir -p animategraphics
#ln -s ../0/UT.pdf animategraphics/T_0.pdf
for time in [0-9]*; do
    let t=$time/100000
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

