#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh

# Generate the mesh
blockMesh

# Calculate the initial conditions and the fluxes before imposing the terrain
cp -r init0 0
solidBodyRotationOnPlaneSetup

# Put in a mountain
terrainFollowingMesh

# Write out cell volumes for post-processing
time=0
postProcess -func writeCellVolumes -time $time
gmtFoam -time $time Tunder
gmtFoam -time $time mountainOver
cat $time/Tunder.ps $time/mountainOver.ps > $time/Tmountain.ps
ps2pdf $time/Tmountain.ps
mv Tmountain.pdf $time/Tmountain.pdf
rm $time/*.ps
evince $time/Tmountain.pdf &

rm 0/U 0/Uf

# Run
scalarTransportFoam | tee log.txt

# Plot results
for time in [1-9]*; do
    ln -s ../0/Uf $time/Uf
    ln -s ../0/U $time/U
    ln -s ../0/V $time/V
done

gmtFoam UT
gmtFoam T
eps2gif UT.gif 0/UT.pdf ??/UT.pdf ???/UT.pdf

# Make links for animategraphics
field=T
mkdir -p animategraphics
ln -s ../0/UT.pdf animategraphics/T_0.pdf
for time in [1-9]*; do
    let t=$time/50
    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
done

