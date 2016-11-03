#!/bin/bash

set -o verbose


read -p "Press [Enter] key to run blockMesh: "
blockMesh


read -p "Press [Enter] key to run paraFoam: "

# remove errors in the boundary necessary for paraFoam
sed -i '/1 ( zeroGradient )/d' constant/polyMesh/boundary

paraFoam -builtin &


#look at colouring mesh by vtkBlockRegion in paraFoam
