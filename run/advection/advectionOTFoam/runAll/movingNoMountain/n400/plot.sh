#!/bin/bash -e

# Post process
for field in mesh A T uniT monitor; do
    gmtFoam ${field}
done
gmtFoam meshOverMountain

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in mesh A T uniT monitor meshOverMountain; do
	ln -s ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in mesh A T uniT monitor meshOverMountain; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done

# Make gif animation
for field in mesh A T uniT monitor meshOverMountain; do
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf ????/$field.pdf
done
