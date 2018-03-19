# Run and post process all advectionOTFoam cases

# run all cases
for case in movingOverMountainsA movingOverMountains fixedOverMountains \
            fixedMesh movingNoMountain; do
    cd $case
    ./run.sh
    cd ..
done

# Create mountain plot
cd ../drawMountain
./run.sh
cd ../adveadvectionOT

# Create plots of for all cases with mountains
gmtFoam -case ../drawMountain 
for case in movingOverMountainsA movingOverMountains fixedOverMountains; do
    for field in T uniT monitor A; do
        gmtFoam ${field}under -case $case
        for time in $case/[0-9]*; do
            time=`filename $time`
            cat $case/$time/${field}under.ps \
                ../drawMountain/0/mountainOver.ps > $case/$time/${field}.ps
            ps2pdf $case/$time/${field}.ps $case/$time/${field}.ps.pdf
            pdfcrop $case/$time/${field}.ps.pdf $case/$time/${field}.pdf
            rm $case/$time/${field}under.ps $case/$time/${field}.ps \
                $case/$time/${field}.ps.pdf
        done
    done
done

# Create plots of for all cases without mountains
for case in movingNoMountain fixedMesh; do
    for field in T monitor; do
        gmtFoam ${field} -case $case
    done
done

# Make links for animategraphics
for case in movingOverMountainsA movingOverMountains fixedOverMountains; do
    mkdir -p $case/animategraphics
    time=0
    for field in A T uniT monitor; do
        ln -s ../$time/$field.pdf $case/animategraphics/${field}_$time.pdf
    done

    for field in A T uniT monitor; do
        for time in $case/[1-9]*; do
            time=`filename $time`
            t=`echo $time | awk {'print $1/50'}`
            ln -s ../$time/$field.pdf $case/animategraphics/${field}_$t.pdf
        done
    done
done

for case in movingNoMountain fixedMesh; do
    mkdir -p $case/animategraphics
    time=0
    for field in T monitor; do
        ln -s ../$time/$field.pdf $case/animategraphics/${field}_$time.pdf
    done

    for field in T monitor; do
        for time in $case/[1-9]*; do
            time=`filename $time`
            t=`echo $time | awk {'print $1/50'}`
            ln -s ../$time/$field.pdf $case/animategraphics/${field}_$t.pdf
        done
    done
done

