#!/bin/bash -e

# Post process
for time in [0-9]*; do
    gmtFoam -time $time mesh
    gmtFoam -time $time T
done

# Conservation of T
globalSum T

# Make links for animategraphics
mkdir -p animategraphics
for field in mesh T; do
    for time in [0-9]*; do
	t=`echo $time | awk {'print $1/50'}`
	ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
