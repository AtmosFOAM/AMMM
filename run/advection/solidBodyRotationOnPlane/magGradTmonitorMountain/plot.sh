#!/bin/bash -e

# Plot results
for field in A T uniT flowOverGround UT mesh monitor; do
    gmtFoam ${field}under -region pMesh
    for time in [0-9]*; do
	cat $time/${field}under.ps ../drawMountain/0/mountainOver.ps \
            > $time/${field}.ps
        ps2pdf $time/${field}.ps $time/${field}.pdf.pdf
        pdfcrop $time/${field}.pdf.pdf $time/${field}.pdf
        rm $time/${field}under.ps $time/${field}.ps $time/${field}.pdf.pdf
        gv $time/${field}.pdf &
    done
done

# Conservation of T
globalSum A -region pMesh
globalSum T -region pMesh
globalSum uniT -region pMesh

# Make links for animategraphics
mkdir -p animategraphics
for field in A T uniT flowOverGround UT mesh monitor; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1/50'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done
