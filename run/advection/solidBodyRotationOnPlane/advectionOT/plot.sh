#!/bin/bash -e

# Post process
for time in [0-9]*; do
    gmtFoam -time $time mesh
    gmtFoam -time $time T
    gmtFoam -time $time uniT
done

# Conservation of T
globalSum T
globalSum uniT

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in mesh T uniT; do
	    ln -s ../$time/$field.pdf animategraphics/${field}_$time.pdf
    done
done

for field in mesh T uniT; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
