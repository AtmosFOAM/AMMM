#!/bin/bash -e

# Remove old stuff
rm -rf [0-9]* constant/polyMesh constant/rMesh constant/pMesh

# Generate the mesh - the rMesh and pMesh are periodic but the 
# mesh is not periodic
blockMesh
blockMesh -region rMesh
blockMesh -region pMesh

# Calculate the initial conditions before imposing the terrain
cp -r init0 0
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

# Iterate, creating an adapted mesh and initial conditions on the mesh
meshIter=0
until [ $meshIter -ge 10 ]; do
    echo Mesh generation iteration $meshIter
    
    # Calculate the rMesh based on the monitor function derived from Uf
    movingScalarTransportFoam -reMeshOnly

    # Re-create the initial conditions
    setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                           -tracerDict tracerDict -name T
    
    let meshIter+=1
done
# Re-create velocity field
setVelocityField -region pMesh -dict advectionDict
setAnalyticTracerField -region pMesh -velocityDict advectionDict \
                       -tracerDict tracerDict -name T

# Raise the mountain
terrainFollowingMesh -region pMesh

gmtFoam -time 0 -region pMesh UTmesh
evince 0/UTmesh.pdf &
gmtFoam -time 0 -region pMesh UT
gmtFoam -time 0 -region pMesh monitor
evince 0/monitor.pdf &

# Run
movingScalarTransportFoam >& log & sleep 0.001; tail -f log

# Plot results
for field in T UT mesh uniT; do
    gmtFoam ${field}under -region pMesh
    for time in [0-9]*; do
        cat $time/${field}under.ps ../fixedMountain/0/mountainOver.ps \
            > $time/${field}.ps
        ps2pdf $time/${field}.ps $time/${field}.pdf.pdf
        pdfcrop $time/${field}.pdf.pdf $time/${field}.pdf
        rm $time/${field}under.ps $time/${field}.ps $time/${field}.pdf.pdf
        gv $time/${field}.pdf &
    done
done

# Conservation of T
globalSum T -region pMesh

# Make links for animategraphics
mkdir -p animategraphics
ln -s ../0/UT.pdf animategraphics/T_0.pdf
for field in T mesh uniT; do
    for time in [0-9]*; do
        t=`echo $time | awk {'print $1*4'}`
        ln -s ../$time/$field.pdf animategraphics/${field}_$t.pdf
    done
done

