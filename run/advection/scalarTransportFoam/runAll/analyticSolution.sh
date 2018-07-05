#!/bin/bash -ve

if [ "$#" -ne 4 ]
then
   echo usage: analyticSolution.sh orthogonal'|'nonOrthog nx deltaT endTime
   exit
fi

type=$1
nx=$2
dt=$3
endTime=$4

case=$type/${nx}x${nx}
acase=$case/analytic
rm -rf $acase
mkdir -p $acase

# derived properties
# number of grid points in half of the domain
let NX=$nx/2
SQUEEZE=1
EXPAND=1
AM=5
AP=5
if [ "$type" == "orthogonal" ]; then
    echo orthogonal case
elif [ "$type" == "nonOrthog" ]; then
    echo non-orthogonal case
    SQUEEZE=0.5
    EXPAND=2
    AM=3.55662432703
    AP=6.44337567297
else
    echo in analyticSolution.sh type<orthogonal|nonOrthog> nx deltaT endTime
    echo type must be one of orthogonal or nonOrthog, not $type
    exit
fi

# Generate the mesh
cp -r dummy/system dummy/constant dummy/0 $acase
sed -i -e 's/NX/'$NX'/g' -e 's/AM/'$AM'/g' -e 's/AP/'$AP'/g' \
    -e 's/SQUEEZE/'$SQUEEZE'/g' -e 's/EXPAND/'$EXPAND'/g' \
    $acase/constant/polyMesh/blockMeshDict
sed -e 's/DELTAT/'$dt'/g' -e 's/STARTTIME/0/g' -e 's/ENDTIME/'$endTime'/g' \
    dummy/system/controlDict > $acase/system/controlDict
blockMesh -case $acase
if [ $nx -le 100 ]; then
    gmtFoam -case $acase -constant mesh
    gv $acase/constant/mesh.pdf &
fi

# Calculate the analytic solution at every output time
time=0
while [ "$time" -le "$endTime" ]; do
    sed -e 's/DELTAT/'$dt'/g' -e 's/STARTTIME/'$time'/g' -e 's/ENDTIME/'$endTime'/g' \
        dummy/system/controlDict > $acase/system/controlDict
    cp -r dummy/0 $acase/$time
    solidBodyRotationOnPlaneSetup -case $acase
    let time=$time+$dt
done

if [ $nx -le 100 ]; then
    gmtFoam -case $acase -time 0 UT
    gv $acase/0/UT.pdf &
fi

# Calculate stats for error measures
globalSum -case $acase T

