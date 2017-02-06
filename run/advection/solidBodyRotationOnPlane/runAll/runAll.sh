#!/bin/bash -e

# Generate the analytic solutions for all grids
for type in orthogonal nonOrthog; do
    for nx in 50 100 200 400; do
        ./runAll/analyticSolution.sh $type $nx 100 600
    done
done


# set up all of the test cases
# ./runAll/init.sh orthogonal|nonOrthog nx dt
./runAll/init.sh orthogonal 50 2 600
./runAll/init.sh orthogonal 100 1 600
./runAll/init.sh orthogonal 200 0.5 600
./runAll/init.sh orthogonal 400 0.25 600

./runAll/init.sh nonOrthog 50 2 600
./runAll/init.sh nonOrthog 100 1 600
./runAll/init.sh nonOrthog 200 0.5 600
./runAll/init.sh nonOrthog 400 0.25 600

# set up test cases with long time steps
./runAll/init.sh orthogonal 50 20 600
./runAll/init.sh orthogonal 100 10 600
./runAll/init.sh orthogonal 200 5 600
./runAll/init.sh orthogonal 400 2.5 600

./runAll/init.sh nonOrthog 50 20 600
./runAll/init.sh nonOrthog 100 10 600
./runAll/init.sh nonOrthog 200 5 600
./runAll/init.sh nonOrthog 400 2.5 600

# sensitivity to time-step
./runAll/init.sh orthogonal 100 0.25 600
./runAll/init.sh orthogonal 100 0.5 600
./runAll/init.sh orthogonal 100 2 600
./runAll/init.sh orthogonal 100 5 600
./runAll/init.sh nonOrthog 100 0.25 600
./runAll/init.sh nonOrthog 100 0.5 600
./runAll/init.sh nonOrthog 100 2 600
./runAll/init.sh nonOrthog 100 5 600

# run all test cases
for case in */[1-9]*/dt* ; do
#    scalarTransportRKGSTFoam -case $case >& $case/log &
    scalarTransportFoamCN -case $case >& $case/log &
done

# Cacluate error measures for all cases
for case in */[1-9]*/dt*; do
    ./runAll/calcErrors.sh $case
done

gmtPlot runAll/plotl2.gmt
gmtPlot runAll/plotlinf.gmt

# Plot a load of results
for case in */100x100/dt* */400x400/dt_2.5 ; do
    ./runAll/plotResults.sh $case
done

# plot errors at T=5 as a function of dx
mkdir -p runAll/data
echo -e "#dx error1st\n20 0.1\n200 1" > runAll/data/1stOrder.dat
echo -e "#dx error2nd\n20 0.005\n200 0.5" > runAll/data/2ndOrder.dat
echo -e "#dx error3rd\n20 1e-4\n200 0.1" > runAll/data/3rdOrder.dat
DX=10000
for type in orthogonal nonOrthog; do
    for c in 1 10; do
        echo '#dx l2 linf' > $type/errorNorms_c$c.dat
        for nx in 50 100 200 400; do
            dt=`echo $c $nx | awk '{print $1*100/$2}'`
            let dx=$DX/$nx
            case=$type/${nx}x${nx}/dt_$dt
            echo $type $dx $dt $nx $case
            l2=`tail -2 $case/errorNorms.dat | head -1 | awk '{print $3}'`
            linf=`tail -2 $case/errorNorms.dat | head -1 | awk '{print $4}'`
            echo $dx $l2 $linf >> $type/errorNorms_c$c.dat
        done
    done
done

gmtPlot runAll/plotl2ConvergenceC1.gmt
gmtPlot runAll/plotl2ConvergenceC10.gmt
gmtPlot runAll/plotlinfConvergenceC1.gmt
gmtPlot runAll/plotlinfConvergenceC10.gmt

# assemble errors at T=500 for convergence with time-step
res=100x100
for type in orthogonal nonOrthog; do
    echo '#dt l2 linf' > runAll/data/${type}_${res}_errorNorms.dat
    for dt in 0.25 0.5 1 2 5 10; do
        dir=${type}/$res/dt_${dt}
        tail -2 $dir/errorNorms.dat | head -1 | awk '{print '$dt', $3, $4}' >>\
            runAll/data/${type}_${res}_errorNorms.dat
    done
done

# plots for convergence with time-step
gmtPlot runAll/plotl2_dt.gmt
gmtPlot runAll/plotlinf_dt.gmt

