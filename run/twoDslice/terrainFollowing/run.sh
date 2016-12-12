## this code will create a "frontal" mesh moving over orography

rm -rf [0-9]* constant/*Mesh
rm -rf 0/polyMesh 0/rMesh 0/h0 0/pMesh
rm -rf constant/rMesh/polyMesh/ constant/pMesh/polyMesh
blockMesh

# remove errors in the boundary
sed -i '/1 ( zeroGradient )/d' constant/polyMesh/boundary

rm -rf 0/meshPhi
mkdir -p constant/rMesh/polyMesh/
mkdir -p constant/pMesh/polyMesh/
cp -p constant/polyMesh/* constant/rMesh/polyMesh/.
cp -p constant/polyMesh/* constant/pMesh/polyMesh/.
mkdir -p 0/pMesh
cp constant/U0 0/pMesh/U
cp constant/mmPhi 0/mmPhi

meshMovement2Dslice
