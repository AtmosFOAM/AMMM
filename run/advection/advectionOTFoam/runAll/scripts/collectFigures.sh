#!/bin/bash 
#
# Collect figures for the paper and put them in ../figures

mkdir -p figures
cp fixedOverMountains/n100/0/T.pdf figures/initial_tracer.pdf
cp fixedOverMountains/n100/0/U.pdf figures/initial_velocity.pdf

cp movingOverMountains/n100/0/meshOverMountain.pdf figures/orography_slice.pdf

cp movingOverMountains/n100/150/T.pdf figures/tracer_150.pdf
cp movingOverMountains/n100/300/T.pdf figures/tracer_300.pdf
cp movingOverMountains/n100/450/T.pdf figures/tracer_450.pdf
cp movingOverMountains/n100/600/T.pdf figures/tracer_600.pdf

cp movingOverMountains/n100/150/mesh.pdf figures/mesh_150.pdf
cp movingOverMountains/n100/300/mesh.pdf figures/mesh_300.pdf
cp movingOverMountains/n100/450/mesh.pdf figures/mesh_450.pdf
cp movingOverMountains/n100/600/mesh.pdf figures/mesh_600.pdf

cp movingOverMountains/n100/plots/Vchange.pdf figures/Vchange.pdf

cp movingOverMountains/n100/50/uniT.pdf figures/uniT_50.pdf
cp movingOverMountains/n100/150/uniT.pdf figures/uniT_150.pdf
cp movingOverMountains/n100/300/uniT.pdf figures/uniT_300.pdf
cp movingOverMountains/n100/450/uniT.pdf figures/uniT_450.pdf
cp movingOverMountains/n100/600/uniT.pdf figures/uniT_600.pdf

cp movingOverMountainsA/n100/150/A.pdf figures/A_150.pdf
cp movingOverMountainsA/n100/300/A.pdf figures/A_300.pdf
cp movingOverMountainsA/n100/450/A.pdf figures/A_450.pdf
cp movingOverMountainsA/n100/600/A.pdf figures/A_600.pdf

cp movingOverMountainsA/n100/plots/ATchange.pdf figures/ATchange.pdf

cp movingOverMountainsA/n100_long/plots/AminMax.pdf figures/AminMax.pdf

cp l2norms.pdf figures/l2norms.pdf

for file in plots/*eps; do
    filename=`filename $file`
    fileroot=`fileroot $filename`
    eps2pdf $file
    mv plots/$fileroot.pdf figures
done

