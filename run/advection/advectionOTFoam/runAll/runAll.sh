# Run and post process all advectionOTFoam cases

# run all cases
for case in movingOverMountainsA movingOverMountains fixedOverMountains \
            fixedNoMountain movingNoMountain; do
    cd $case
    ./run.sh
    cd ..
done

# Create mountain plot
cd ../drawMountain
./run.sh
cd ../runAll

# Create plots of for all cases with mountains
for case in movingOverMountainsA movingOverMountains fixedOverMountains; do
    gmtFoam -case $case meshOverMountain
    for field in T uniT monitor A; do
        gmtFoam ${field}under -case $case
        for time in $case/[0-9]*; do
            cat $time/${field}under.ps \
                ../drawMountain/0/mountainOver.ps > $time/${field}.ps
            ps2pdf $time/${field}.ps $time/${field}.ps.pdf
            pdfcrop $time/${field}.ps.pdf $time/${field}.pdf
            rm $time/${field}under.ps $time/${field}.ps \
                $time/${field}.ps.pdf
        done
    done
done

# Create plots of for all cases without mountains
for case in movingNoMountain fixedNoMountain; do
    for field in T monitor; do
        gmtFoam ${field} -case $case
    done
done

# Make links for animategraphics
for case in movingOverMountainsA movingOverMountains fixedOverMountains; do
    mkdir -p $case/animategraphics
    time=0
    for field in A T uniT monitor meshOverMountain; do
        ln -s ../$time/$field.pdf $case/animategraphics/${field}_$time.pdf
    done

    for field in A T uniT monitor meshOverMountain; do
        cd $case
        for time in [1-9]*; do
            t=`echo $time | awk {'print $1/50'}`
            ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
        done
        cd ..
    done
done

for case in movingNoMountain fixedNoMountain; do
    mkdir -p $case/animategraphics
    time=0
    for field in T monitor; do
        ln -s ../$time/$field.pdf $case/animategraphics/${field}_$time.pdf
    done

    for field in T monitor; do
        cd $case
        for time in [1-9]*; do
            t=`echo $time | awk {'print $1/50'}`
            ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
        done
        cd ..
    done
done

# Create graphs
for dir in movingOverMountains movingOverMountainsA; do
    cd $dir
    mkdir plots
    gmtPlot ../plots/Vchange.gmt
    gmtPlot ../plots/Tchange.gmt
    gmtPlot ../plots/ATchange.gmt
    grep '\bTime =' log | awk '{print $3}' > time.dat
    grep '\bT goes from' log | awk '{print $4, $6}' > T.dat
    grep '\buniT goes from' log | awk '{print $4, $6}' > uniT.dat
    grep '\bA goes from ' log | awk '{print $4, $6}' > A.dat
    echo '#time Tmin Tmax uniTmin uniTmax Amin Amax' > minMax.dat
    paste time.dat T.dat uniT.dat A.dat >> minMax.dat
    rm time.dat T.dat uniT.dat A.dat
    gmtPlot ../plots/uniT.gmt
    gmtPlot ../plots/A.gmt
    cd ..
done
