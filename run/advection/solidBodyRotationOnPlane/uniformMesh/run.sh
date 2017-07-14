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
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

gmtFoam -time 0 -region pMesh UT
evince 0/UT.pdf &

# Run
movingScalarTransportFoam -fixedMesh | tee log.txt

# Plot results
time=0
field=T
gmtFoam $field -region pMesh -time $time':'
eps2gif $field.gif 0/$field.pdf ??/$field.pdf ???/$field.pdf

# Conservation of T
globalSum T -region pMesh

# Make links for animategraphics
field=T
mkdir -p animategraphics
ln -s ../0/UT.pdf animategraphics/T_0.pdf
for time in [1-9]*; do
    let t=$time/50
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

