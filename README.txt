# AMMM a repository for the codes used in the Adaptive Moving Mesh Methods NERC Project. This repo is a fork of AtmosFOAM:

# AtmosFOAM
A collection of OpenFOAM computational fluid dynamics applications and libraries for performing atmospheric experiments.  Includes mesh generators, scalar transport and Navier-Stokes solvers, and post-processing and visualisation tools.

## Installation
First, install [OpenFOAM 3.0.1](http://www.openfoam.org/download/) or later.
Second, set the variable the ATMOSFOAM_SRC in the .bashrc file:
- export ATMOSFOAM_SRC=$HOME/$WM_PROJECT/$USER-3.0.1/AMMM/src
Third, run Allwmake to build the codes

Compile all AtmosFOAM applications and libraries using `./Allwmake`

