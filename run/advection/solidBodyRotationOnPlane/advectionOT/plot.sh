#!/bin/bash -e

# Post process
for time in [0-9]*; do
    gmtFoam -time $time mesh
    gmtFoam -time $time T
    gmtFoam -time $time uniT
    gmtFoam -time $time monitor
done

# Conservation of T
globalSum T
globalSum uniT
globalSum AT

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in mesh T uniT monitor; do
	ln -s ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in mesh T uniT monitor; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
