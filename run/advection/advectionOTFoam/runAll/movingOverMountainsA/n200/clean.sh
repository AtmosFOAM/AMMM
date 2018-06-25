#!/bin/bash -e
# Clean the case

foamListTimes -withZero -rm
rm -rf constant/cMesh constant/polyMesh constant/T_init legends gmt.history
