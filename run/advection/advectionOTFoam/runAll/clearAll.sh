# Clear all advectionOTFoam cases

for case in movingOverMountainsA movingOverMountains fixedOverMountains \
            fixedNoMountain movingNoMountain; do
    cd $case
    foamListTimes -withZero -rm
    rm -rf constant/cMesh constant/polyMesh constant/T_init legends gmt.history
    rm -rf animategraphics plots log diagnostics.dat minMax.dat
    cd ..
done
