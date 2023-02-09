#!/bin/bash -e

# Plot monitor function and cell volume
time=600
postProcess -func writeCellVolumes -time $time
gmtFoam -time $time monitor
gv $time/monitor.pdf &

## Create mountainOver.ps
#cp system/mountainDict ../../../drawMountain/system/mountainDict
#cd ../../../drawMountain
#./run.sh
#cd -

# Post process
for field in mesh A T uniT monitor UT; do
    gmtFoam ${field}under
    for time in [0-9]*; do
        cat $time/${field}under.ps ../../../drawMountain/0/mountainOver.ps \
            > $time/${field}.ps
        ps2pdf $time/${field}.ps $time/${field}.pdf.pdf
        pdfcrop $time/${field}.pdf.pdf $time/${field}.pdf
        rm $time/${field}under.ps $time/${field}.ps $time/${field}.pdf.pdf
    done
done
gmtFoam meshOverMountain

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in mesh A T uniT monitor meshOverMountain; do
	ln -sf ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in mesh A T uniT monitor meshOverMountain; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -sf ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done

# Make gif animation
for field in mesh A T uniT monitor meshOverMountain; do
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf
done

