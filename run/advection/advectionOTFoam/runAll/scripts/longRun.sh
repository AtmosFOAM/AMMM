#!/bin/bash -ev
#
# Run movingOverMountains and movingOverMountainsA cases for 10 circulations

# run all cases
for case in movingOverMountains movingOverMountainsA; do
    cd $case/n100_long
    ./run.sh
    cd ../..
done

# Create mountain plot
cd ../drawMountain
./run.sh
cd ../runAll

# Create plots
for case in movingOverMountainsA movingOverMountains; do
    cd $case
    gmtFoam -case n100_long meshOverMountain
    for field in T uniT monitor A; do
        gmtFoam ${field}under -case n100_long
        for time in n100_long/[0-9]*; do
            cat $time/${field}under.ps \
                ../../drawMountain/0/mountainOver.ps > $time/${field}.ps
            ps2pdf $time/${field}.ps $time/${field}.ps.pdf
            pdfcrop $time/${field}.ps.pdf $time/${field}.pdf
            rm $time/${field}under.ps $time/${field}.ps \
                $time/${field}.ps.pdf
        done
    done
    cd ..
done

# Make links for animategraphics
for case in movingOverMountainsA movingOverMountains; do
    mkdir -p $case/n100_long/animategraphics
    time=0
    for field in A T uniT monitor meshOverMountain; do
        ln -sf ../$time/$field.pdf $case/n100_long/animategraphics/${field}_$time.pdf
    done

    for field in A T uniT monitor meshOverMountain; do
        cd $case/n100_long
        for time in [1-9]*; do
            t=`echo $time | awk {'print $1/50'}`
            ln -sf ../$time/$field.pdf animategraphics/${field}_$t.pdf
        done
        cd ../..
    done
done

# Create graphs
for dir in movingOverMountains movingOverMountainsA; do
    cd $dir/n100_long
    mkdir -p plots
    gmtPlot ../../plots/longRun/Vchange.gmt
    gmtPlot ../../plots/longRun/Tchange.gmt
    gmtPlot ../../plots/longRun/ATchange.gmt
    grep '\bTime =' log | awk '{print $3}' > time.dat
    grep '\bT goes from' log | awk '{print $4, $6}' > T.dat
    grep '\buniT goes from' log | awk '{print $4, $6}' > uniT.dat
    grep '\bA goes from ' log | awk '{print $4, $6}' > A.dat
    echo '#time Tmin Tmax uniTmin uniTmax Amin Amax' > minMax.dat
    paste time.dat T.dat uniT.dat A.dat >> minMax.dat
    rm time.dat T.dat uniT.dat A.dat
    gmtPlot ../../plots/longRun/uniT.gmt
    gmtPlot ../../plots/longRun/A.gmt
    cd ../..
done
