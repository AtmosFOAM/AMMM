#!/bin/bash -e

if [ "$#" -ne 4 ]
then
   echo usage: init.sh orthogonal'|'nonOrthog nx deltaT endTime
   exit
fi

type=$1
nx=$2
dt=$3
endTime=$4
case=$type/${nx}x${nx}/dt_$dt

rm -rf $case
mkdir $case

# Link with the mesh and initial conditions from the analytic solution
ln -s ../analytic/0 $case/0
ln -s ../analytic/constant $case/constant

# Create a case with the correct time-step
cp -r dummy/system $case
sed -e 's/DELTAT/'$dt'/g' -e 's/STARTTIME/0/g' -e 's/ENDTIME/'$endTime'/g' \
    dummy/system/controlDict > $case/system/controlDict

