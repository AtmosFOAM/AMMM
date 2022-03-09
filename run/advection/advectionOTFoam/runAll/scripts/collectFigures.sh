#!/bin/bash 
#
# Collect figures for the paper and put them in ../figures

mkdir -p figures
cp movingOverMountains/n100/0/T.pdf figures/initial_tracer.pdf
cp fixedOverMountains/n100/0/U.pdf figures/initial_velocity.pdf

cp movingOverMountains/n100/0/mesh.pdf figures/initial_mesh.pdf
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

cp movingOverMountainsA/n100/plots/AVchange.pdf figures/AVchange.pdf

cp movingOverMountainsA/n100_long/plots/AminMax.pdf figures/AminMax.pdf

cp l2norms.pdf figures/l2norms.pdf

for file in plots/*eps; do
    filename=`filename $file`
    fileroot=`fileroot $filename`
    epstopdf $file
    mv plots/$fileroot.pdf figures
    pdftk figures/$fileroot.pdf cat 1-endeast output figures/output.pdf
    mv figures/output.pdf figures/$fileroot.pdf
done

cd figures
pdftk AminMax.pdf cat 1-endeast output AminMax_curl.pdf
pdftk Vchange.pdf cat 1-endeast output Vchange_curl.pdf
pdftk AVchange.pdf cat 1-endeast output AVchange_curl.pdf
pdftk l2norms.pdf cat 1-endeast output l2norms_curl.pdf
mv AminMax_curl.pdf AminMax.pdf
mv Vchange_curl.pdf Vchange.pdf
mv AVchange_curl.pdf AVchange.pdf
mv l2norms_curl.pdf l2norms.pdf
