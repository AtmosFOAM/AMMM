#!/bin/bash -ve

# Copy old case and compare differences
cp -r ../gradVorticityMonitor2_5050/0 .

# Solve the incompressible Euler
movingIcoFoamA

# Compare with a saved solution
for time in 200 1000; do
    for var in monitor p U; do
        sumFields -region rMesh $time ${var}Diff $time $var 10$time $var -scale1 -1
        echo That was differences for $var at time $time. 
        read -p "Press return to continue"
    done
done

