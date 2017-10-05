#!/bin/bash -e
# Clean the case

foamListTimes -withZero -rm
rm -rf constant/cMesh constant/polyMesh legends gmt.history
