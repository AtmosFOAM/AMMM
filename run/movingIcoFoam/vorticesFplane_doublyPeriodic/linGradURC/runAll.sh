#!/bin/bash -ve

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/*/polyMesh constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh

# Create initial conditions on the rMesh and plot
time=0
cp -r init_0 0
# Create Gaussian patches of voriticty
setGaussians initDict -region rMesh
# Invert to find the wind field
invertVorticity -time $time initDict -region rMesh

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 5 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    movingIcoFoamA -reMeshOnly

    # Re-create the initial conditions and re-plot
    setGaussians initDict -region rMesh
    # Invert to find the wind field
    invertVorticity -time $time initDict -region rMesh

    let meshIter+=1
done

time=0
postProcess -func vorticity -region rMesh -time $time
writeuvw -time $time -region rMesh vorticity
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

gmtFoam -time $time monitor -region rMesh
evince $time/monitor.pdf &

# Solve the SWE
movingIcoFoamA >& log & sleep 0.01; tail -f log

# Post process
time=700000

gmtFoam -time $time -region rMesh pUmesh
gv $time/pUmesh.pdf &

postProcess -func vorticity -time $time -region rMesh
writeuvw -time $time vorticity -region rMesh
gmtFoam -time $time vorticity -region rMesh
evince $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=200000
postProcess -func vorticity -time $time':'
writeuvw vorticity -time $time':'
gmtFoam vorticity -time $time':'


# Animation of vorticity
postProcess -func vorticity -region rMesh
writeuvw vorticity -region rMesh
gmtFoam vorticityMesh -region rMesh
eps2gif vorticityMesh.gif 0/vorticityMesh.pdf ??????/vorticityMesh.pdf ???????/vorticityMesh.pdf

# Animation of the mesh
gmtFoam -region rMesh mesh
eps2gif mesh.gif 0/mesh.pdf ??????/mesh.pdf ???????/mesh.pdf

# Make links for animategraphics
field=vorticityMesh
DT=100000
mkdir -p animategraphics
for time in [0-9]*; do
    let t=$time/$DT
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

