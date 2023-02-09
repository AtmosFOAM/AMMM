#!/bin/bash -e

# Moving mesh
cp 600/polyMesh/points constant/polyMesh
cp 600/T 600/Ttmp
mv 0 0tmp
cp -r init0 0
cp 0/T constant/T_init
setAnalyticTracerField -time 0 \
                       -velocityDict advectionDict \
                       -tracerDict tracerDict -name T
mv 600/Ttmp 600/T
mv 0/T 600/Tanalytic
rm -rf 0
mv 0tmp 0
sumFields 600 Terror 600 T 600 Tanalytic -scale1 -1
globalSum -time 600 Terror

# Plotting difference
# gmtFoam -time 0 T
# gmtFoam -time 600 T
# gmtFoam -time 600 Terror
# gv 0/T.pdf &
# gv 600/T.pdf &
# gv 600/Terror.pdf &

# Finding and storing the max Courant number and min and max dx
dt=`grep deltaT system/controlDict \
    | awk '{print $2}' | awk -F';' '{print $1}'`
maxC=`grep Courant log \
    | awk 'BEGIN{maxC=0} {if ($6 > maxC) maxC=$6} END{print maxC}'`
meshAnalysis2D tracerDict -time 0
mindx=`awk 'BEGIN {min=1000} {if ($2 < min) min=$2; } END {print min}'\
     0/distMetrics.dat`
maxdx=`awk 'BEGIN {max=0} {if ($2 > max && $2==1*$2) max=$2} \
    END {print  max}' 0/distMetrics.dat`
echo -e "dt $dt\nmaxC $maxC\nmindx $mindx\nmaxdx $maxdx" > runStats.dat

