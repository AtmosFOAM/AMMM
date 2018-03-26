#!/bin/bash -e

# Create analytic solution
cp 0/T 600/Ta

# Draw analytical condition
gmtFoam -time 600 T
gmtFoam -time 600 Ta
gv 600/T.pdf &
gv 600/Ta.pdf &

# Create Terror
sumFields 600 Terror 600 T 600 Ta -scale1 -1
gmtFoam -time 600 Terror
gv 600/Terror.pdf &

# Calculate norms
globalSum -time 600 Ta
globalSum -time 600 Terror
