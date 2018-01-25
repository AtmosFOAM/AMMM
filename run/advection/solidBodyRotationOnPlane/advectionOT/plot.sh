#!/bin/bash -e

# Post process
for field in mesh A T uniT monitor; do
    gmtFoam $field
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf ????/$field.pdf
done

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in mesh A T uniT monitor; do
	ln -s ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in mesh A T uniT monitor; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/100'}`
	    ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
