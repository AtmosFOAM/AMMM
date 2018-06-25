#!/bin/bash -e

# Fixed mesh
sumFields 600 Terror 600 T 0 T -scale1 -1
globalSum -time 600 Terror

# Plotting difference
# gmtFoam -time 0 T
# gmtFoam -time 600 T
# gmtFoam -time 600 Terror
# gv 0/T.pdf &
# gv 600/T.pdf &
# gv 600/Terror.pdf &
