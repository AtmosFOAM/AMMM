#!/bin/bash -e

## Create mountainOver.ps
#cp system/mountainDict ../../../drawMountain/system/mountainDict
#cd ../../../drawMountain
#./run.sh
#cd -

# Remove 1 from uniT for more accurate post processing
for time in [1-9]*; do
    sumFields $time uniTdiff $time uniT 0 uniT -scale0 1e12 -scale1 -1e12
done

# Post process
for field in  A uniTdiff Tmonitor; do
    for time in [0-9]*; do
        gmtFoam ${field}under -time $time; \
        cat $time/${field}under.ps ../../../drawMountain/0/mountainOver.ps \
            > $time/${field}.ps; \
        ps2pdf $time/${field}.ps $time/${field}.pdf.pdf; \
        pdfcrop $time/${field}.pdf.pdf $time/${field}.pdf; \
        rm $time/${field}under.ps $time/${field}.ps $time/${field}.pdf.pdf
    done
done
gmtFoam meshOverMountain

# Make links for animategraphics
mkdir -p animategraphics
time=0
for field in A uniTdiff Tmonitor meshOverMountain; do
	ln -sf ../$time/$field.pdf animategraphics/${field}_$time.pdf
done

for field in A uniTdiff Tmonitor meshOverMountain; do
    for time in [1-9]*; do
	    t=`echo $time | awk {'print $1/50'}`
	    ln -sf ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done

# Make gif animation
for field in A uniTdiff Tmonitor meshOverMountain; do
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf
done

