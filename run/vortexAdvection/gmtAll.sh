#!/bin/bash
set -o errexit
set -o nounset

time=$1

for f in constant/gmtDicts/*; do
    g=$(echo $f | rev | cut -f1 -d'/' | rev)
    tidle=$(echo $g | rev | cut -c1 )
    if [[ "$tidle" != "~" ]]; then
       gmtFoam -time $time -region pMesh $g
    fi
done
