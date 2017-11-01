#!/bin/bash -e

# Post process
for time in [0-9]*; do
    gmtFoam -time $time mesh
    gmtFoam -time $time A
    gmtFoam -time $time T
    gmtFoam -time $time AT
    gmtFoam -time $time uniT
    gmtFoam -time $time monitor
    gmtFoam -time $time flowOverGround
done

# Conservation of T
globalSum A
globalSum T
globalSum AT
globalSum uniT


# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in mesh A T AT uniT monitor flowOverGround; do
	ln -s ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in mesh A T AT uniT monitor flowOverGround; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
