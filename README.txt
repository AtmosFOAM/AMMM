# AMMM a repository for the codes used in the Adaptive Moving Mesh Methods NERC Project. This repo is a fork of AtmosFOAM:

# AtmosFOAM
A collection of OpenFOAM computational fluid dynamics applications and libraries for performing atmospheric experiments.  Includes mesh generators, scalar transport and Navier-Stokes solvers, and post-processing and visualisation tools.

## Installation
First, install [OpenFOAM 4 or dev](http://www.openfoam.org/download/).
Second, set the variable the AMMM_SRC in the .bashrc file:
export AMMM_SRC=$HOME/$WM_PROJECT/$USER-$WM_PROJECT_VERSION/AMMM/src

Third, install git repo AtmosFOAM-tools from:

in directory
$HOME/$WM_PROJECT/$USER-$WM_PROJECT_VERSION
and compile AtmosFOAM-tools using Allwmake

Compile all AMMM applications and libraries using `./Allwmake`


