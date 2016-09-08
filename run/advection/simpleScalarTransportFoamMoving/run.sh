#!/bin/bash

set -o errexit
set -o nounset


compile=False
while [[ $# -ge 1 ]]
do
    key="$1"
    echo $key
case $key in
    -c|--compile)
    compile=True
    shift # past argument
    ;;
    *)
            # unknown option
    ;;
esac
done


if [[ "$compile" == "True" ]]; then
    cd $AMMM_SRC/../applications/solvers/advection/simpleScalarTransportFoamMoving
    wmake
    cd -
fi


set -o verbose
#create the mesh
blockMesh

#copy mesh to rMesh
mkdir -p 0/rMesh
mkdir -p constant/rMesh/polyMesh
cp -r constant/polyMesh constant/rMesh

#create initial conditions
setup1D_advection

# set up a mesh velocity
sed 's/U;/rMeshU;/g' 0/rMesh/U | sed 's/.*internalField.*/internalField   uniform (50 0 0);/g' > 0/rMesh/rMeshU

#run the application
simpleScalarTransportFoamOrographyMovingSine | tee log

grep 'Total orography' log | cut -d']' -f2 > orography
grep 'Total T' log | cut -d']' -f2 > tracer

echo 'set terminal "pdf";set output "orography.pdf"; set ylabel "Integral of orography"; set xlabel "Timestep"; plot "./orography" w l notitle' | gnuplot
echo 'set terminal "pdf";set output "tracer.pdf"; set ylabel "Integral of tracer"; set xlabel "Timestep"; plot "./tracer" w l notitle' | gnuplot
