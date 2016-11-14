#!/bin/bash -e
#
# set the following so this script will exit if error happens when piping to tee
set -o pipefail

here=$(pwd)

# Compile the source if necessary
cd $AMMM_SRC/../applications/solvers/incompressible/icoFoamH && wmake && cd -

wd=$(pwd)
if [[ "$wd" != "$here" ]]; then
    #leave if wmake failed
    exit 1
fi

# Create and run an OpenFOAM case for incompressible flow over orography

# clear out old stuff
clearAll.sh
#rm -rf [0-9]* constant/polyMesh core log

# create mesh and plot
blockMesh
terrainFollowingMesh

mkdir -p constant/rMesh/polyMesh constant/pMesh/polyMesh
cp constant/polyMesh/* constant/rMesh/polyMesh/
cp constant/polyMesh/* constant/pMesh/polyMesh/.

mkdir -p 0/pMesh

[ ! -f 0/mesh.pdf ] && gmtFoam -region pMesh mesh
[ -f 0/mesh.pdf ] && evince 0/mesh.pdf &

# initialise the velocity field and the pressure
cp init_0/* 0/
cp init_pMesh/* 0/pMesh/

# Solve incompressible Euler equations
time AMMMicoFoamH | tee log

# plot the results
time=$(ls -1dv [1-9]* | tail -1)
if [ -d $time ]; then
    gmtFoam U -time $time -region pMesh
    evince $time/U.pdf &
    gmtFoam pU -time $time -region pMesh
    evince $time/pU.pdf &
    gmtFoam Ucor -time $time -region pMesh
    evince $time/Ucor.pdf &
fi

if [ -f log ] ; then
    grep 'Courant Number' log | awk '{print $4 " " $6}' > courant_no
    grep 'Solving for p' log  | awk '{print $8 " " $12 " " $15}' | tr ',' ' ' > solver_perf
    echo 'set xlabel "Iteration"; set ylabel "Courant Number"; plot "./courant_no" u 1 w l title "Mean", "./courant_no" u 2 w l title "Max"' | gnuplot --persist
    echo 'set xlabel "iteration"; set ylabel "Residual"; set logscale y; plot "./solver_perf" u 1 w l title "Initial residual", "./solver_perf" u 2 w l title "Final residual"' | gnuplot --persist
fi

