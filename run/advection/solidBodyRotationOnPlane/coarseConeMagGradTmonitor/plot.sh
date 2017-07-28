#!/bin/bash -e

# Plot results
time=0
for field in T UT mesh monitor; do
    gmtFoam $field -region pMesh -time $time':'
done

# Conservation of T
globalSum T -region pMesh

# Make links for animategraphics
mkdir -p animategraphics
for field in T UT mesh monitor; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1/50'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
