#!/bin/bash

set -o verbose

read -p "Press [Enter] key to look at system/blockMeshDict: "
emacs system/blockMeshDict &

read -p " Press [Enter] key to view the blockMesh points using paraview: "

paraFoam -builtin -block &



