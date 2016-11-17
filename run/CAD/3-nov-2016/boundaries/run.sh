#!/bin/bash



emacs system/blockMeshDict &

read -p "Press [Enter] key to run blockMesh: "
blockMesh

read -p "Press [Enter] key to run checkMesh: "
checkMesh


read -p "Press [Enter] key to set up meshes and initial fields: "
terrainFollowingMesh
mkdir -p constant/rMesh/polyMesh constant/pMesh/polyMesh
cp constant/polyMesh/* constant/rMesh/polyMesh/
cp constant/polyMesh/* constant/pMesh/polyMesh/.

mkdir -p 0/pMesh
cp init_0/* 0/pMesh/

read -p "Press [Enter] key to run paraFoam: "
paraFoam -builtin &

read -p "Press [Enter] key to continue: "
clear
cat <<EOF 


There are two *classes* of boundaries in OpenFOAM"
     Geometric boundaries  and  field boundaries"

Geometric boundaries apply to the mesh.
They are set in your system/blockMeshDict

Field boundaries apply to the fields on the meshes.
They are most easily set when reading in the initial fields from 0/

EOF
read -p "Press [Enter] key to continue: "
echo "The available geometric (mesh) boundaries are found in "
echo "$FOAM_SRC/finiteVolume/fields/fvPatchFields/constraint: "
ls -1 $FOAM_SRC/finiteVolume/fields/fvPatchFields/constraint

cat <<EOF 
Plus :
wall
patch
EOF

echo "See http://cfd.direct/openfoam/user-guide/boundaries/ for more info"
echo "or  http://www.openfoam.com/documentation/user-guide/standard-boundaryconditions.php"

read -p "Press [Enter] key to continue: "
cat <<EOF
The boundary conditions for the fields must be consistent with the underlying geometry.

For example, if the mesh has a cyclic boundary patch, then so must the fields.

 
EOF

read -p "Press [Enter] key to see what initial conditions there are: "
ls -R 0

read -p "Press [Enter] key to open the pressure file 0/pMesh/p "
emacs 0/pMesh/p &

read -p "Press [Enter] key to open the velocity file 0/pMesh/Uf "
emacs 0/pMesh/Uf &

read -p "Press [Enter] key to take a look at a createFields.H file: "
emacs $AMMM_SRC/../applications/solvers/incompressible/icoFoamH/createFields.H
