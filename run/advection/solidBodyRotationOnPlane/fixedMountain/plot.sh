#!/bin/bash -e

# Plot results
for time in [1-9]*; do
    ln -s ../0/Uf $time/Uf
    ln -s ../0/U $time/U
    ln -s ../0/V $time/V
done

gmtFoam UT
gmtFoam T
gmtFoam mesh
gmtFoam monitor
#eps2gif UT.gif 0/UT.pdf ??/UT.pdf ???/UT.pdf

# Make links for animategraphics
mkdir -p animategraphics
for field in T UT mesh monitor; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1/50'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
