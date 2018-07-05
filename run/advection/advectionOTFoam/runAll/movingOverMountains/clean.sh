#!/bin/bash -ev
#
# Clean all the cases

rm -f gmt.history
for case in n50 n100 n200 n400 n100_long; do
    cd $case
    foamListTimes -withZero -rm
    rm -rf constant/cMesh constant/polyMesh animategraphics plots
    rm -f constant/T_init legends gmt.history gmt.conf log
    rm -f *.dat *.gif
    cd ..
done
