#!/bin/bash -ve

# Solve the incompressible Euler
icoOTFoamAC >& log & sleep 0.01; tail -f log

# Calculate and plot the vorticity
time=100000
postProcess -func vorticity -time $time
writeuvw -time $time vorticity
gmtFoam -time $time vorticity
evince $time/vorticity.pdf &


# Notes on A-C grid formulation
momentumPredictor not needed
fvm::div(phi, U) not needed

This doesnt work:
U = fvc::reconstruct(phi); 

This does work:
U = HbyA - rAU*fvc::grad(p);

At this resolution,
fvc::interpolate(rAU)*fvc::ddtCorr(U, Uf)
only degrades accuracy

