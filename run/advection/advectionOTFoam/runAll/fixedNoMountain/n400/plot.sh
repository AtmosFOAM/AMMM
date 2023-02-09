#!/bin/bash -e

# Post process
gmtFoam T

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in T; do
	ln -sf ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in T; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -sf ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done

# Make gif animation
for field in T; do
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf
done

