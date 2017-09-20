#!/bin/bash -e

# Post process
case=.
postProcess -func vorticity -case $case
writeuvw vorticity -case $case
for time in [0-9]*; do
    mv $case/$time/vorticityz $case/$time/vorticity
    rm $case/$time/vorticity?
done
gmtFoam vorticity -case $case

for time in [0-9]*; do
    gmtFoam -time $time hU
    gmtFoam -time $time mesh
done

# Animattion of vorticity
# eps2gif vorticity.gif 0/vorticity.pdf ??????/vorticity.pdf \
#                                       ???????/vorticity.pdf

# Make links for animategraphics
mkdir -p animategraphics
for field in vorticity hU mesh; do
    for time in [0-9]*; do
	t=`echo $time | awk {'print $1/100000'}`
	ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
