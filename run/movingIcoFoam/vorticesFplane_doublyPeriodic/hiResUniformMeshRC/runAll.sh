#!/bin/bash -e

# Create the case, run and post-process

# clear out old stuff
rm -rf [0-9]* constant/polyMesh core log legends gmt.history

# Create initial mesh
blockMesh

# Create initial conditions
cp -r init_0 0
# Create Gaussian patches of voriticty
setGaussians initDict
# Invert to find the wind field
invertVorticity -time 0 initDict
mv 0/vorticity 0/vorticityz
gmtFoam -time 0 vorticity
gv 0/vorticity.pdf &

# Solve the incompressibe Euler equations
icoFoam >& log & sleep 0.01; tail -f log

# Post process
time=700000
postProcess -func vorticity -time $time
writeuvw -time $time vorticity
gmtFoam -time $time vorticity
evince $time/vorticity.pdf &

# Only re-calcualte and re-plot recent times
time=2100000
postProcess -func vorticity -time $time':'
writeuvw vorticity -time $time':'
gmtFoam vorticity -time $time':'


# Animation of vorticity
postProcess -func vorticity
writeuvw vorticity
gmtFoam vorticity
eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf ???????/vorticity.pdf

