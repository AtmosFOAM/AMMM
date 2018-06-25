#!/bin/bash -ev
#

# Run and post process all advectionOTFoam cases at 100x100 resolution

# run all cases
for case in movingOverMountainsA movingOverMountains fixedOverMountains \
            fixedNoMountain movingNoMountain; do
    cd $case/n100
    ./run.sh
    cd ../..
done

# Create mountain plot
cd ../drawMountain
./run.sh
cd ../runAll

# Create plots of for all cases with mountains
for case in movingOverMountainsA movingOverMountains fixedOverMountains; do
    cd $case
    gmtFoam -case n100 meshOverMountain
    for field in T uniT monitor A; do
        gmtFoam ${field}under -case n100
        for time in n100/[0-9]*; do
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

# Create plots of for all cases without mountains
for case in movingNoMountain fixedNoMountain; do
    cd $case
    for field in T monitor; do
        gmtFoam ${field} -case n100
    done
    cd ..
done

# Make links for animategraphics
for case in movingOverMountainsA movingOverMountains fixedOverMountains; do
    mkdir -p $case/n100/animategraphics
    time=0
    for field in A T uniT monitor meshOverMountain; do
        ln -s ../$time/$field.pdf $case/n100/animategraphics/${field}_$time.pdf
    done

    for field in A T uniT monitor meshOverMountain; do
        cd $case/n100
        for time in [1-9]*; do
            t=`echo $time | awk {'print $1/50'}`
            ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
        done
        cd ../..
    done
done

for case in movingNoMountain fixedNoMountain; do
    mkdir -p $case/n100/animategraphics
    time=0
    for field in T monitor; do
        ln -s ../$time/$field.pdf $case/n100/animategraphics/${field}_$time.pdf
    done

    for field in T monitor; do
        cd $case/n100
        for time in [1-9]*; do
            t=`echo $time | awk {'print $1/50'}`
            ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
        done
        cd ../..
    done
done

# Create graphs
for dir in movingOverMountains movingOverMountainsA; do
    cd $dir/n100
    mkdir plots
    gmtPlot ../../plots/Vchange.gmt
    gmtPlot ../../plots/Tchange.gmt
    gmtPlot ../../plots/ATchange.gmt
    grep '\bTime =' log | awk '{print $3}' > time.dat
    grep '\bT goes from' log | awk '{print $4, $6}' > T.dat
    grep '\buniT goes from' log | awk '{print $4, $6}' > uniT.dat
    grep '\bA goes from ' log | awk '{print $4, $6}' > A.dat
    echo '#time Tmin Tmax uniTmin uniTmax Amin Amax' > minMax.dat
    paste time.dat T.dat uniT.dat A.dat >> minMax.dat
    rm time.dat T.dat uniT.dat A.dat
    gmtPlot ../../plots/uniT.gmt
    gmtPlot ../../plots/A.gmt
    cd ../..
done
