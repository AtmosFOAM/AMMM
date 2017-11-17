#!/bin/bash -e

# Post process
gmtFoam mesh
gmtFoam A
gmtFoam T
gmtFoam AT
gmtFoam uniT
gmtFoam monitor
gmtFoam flowOverGround

for field in monitor A T mesh; do
    gmtFoam $field
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf ????/$field.pdf
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
