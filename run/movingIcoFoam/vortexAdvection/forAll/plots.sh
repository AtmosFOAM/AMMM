# Approximate time for one complete revolution
time=3150

for case in uniformMesh vorticityMonitor vorticityMonitorFixed magGradUmonitor
do
    echo $case
    gmtFoam -time $time -region rMesh -case $case vorticity
    gv $case/$time/vorticity.pdf &
done

case=vorticityMonitorFixed
gmtFoam -time 0 -region rMesh -case $case vorticity
gv $case/0/vorticity.pdf &

