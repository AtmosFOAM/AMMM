#!/bin/bash
set -o nounset
#set -o errexit
set -o verbose 
set -o xtrace

here=$(pwd)
cd results
cd ring
mkdir -p NEWTON2D-vectorGradc_m
cd NEWTON2D-vectorGradc_m
for N in 50 60 100 150 200 250 300; do
#for N in 250 300; do
    mkdir -p $N
    cd $N
    cp -r /home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/NEWTON2D-vectorGradc_m/0 0
    mkdir -p constant
    cp -r /home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/NEWTON2D-vectorGradc_m/constant/0 constant/0
    cp -r {/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/NEWTON2D-vectorGradc_m/,}constant/polyMesh
    cp -r {/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/NEWTON2D-vectorGradc_m/,}constant/rMesh
    mkdir -p constant/gmtDicts
    cp -r /home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/NEWTON2D-vectorGradc_m/constant/gmtDicts/* constant/gmtDicts/
    mkdir -p system
    cp -r /home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/NEWTON2D-vectorGradc_m/system/{fvSchemes,fvSolution,NEWTON2D-vectorGradc_mDict,controlDict,rMesh,blockMeshDict} system/
    mesh=system/blockMeshDict
    oldres=$(/bin/grep simpleGrading $mesh | cut -d'(' -f3 | cut -d')' -f 1)
    sed -i "s/$oldres/$N $N 1/" $mesh
    ofmesh.sh
    ofmonitor.sh -r
    sed -i 's/^nSmooth.*/nSmooth 0;/g' system/NEWTON2D-vectorGradc_mDict


    NEWTON2D-vectorGradc_m > log 2> err
	grep PABe log | cut -d= -f3 > equi
	grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
    grep -A 1 'Time =' log | grep -B 1 Minimum | grep Time | cut -f2 -d'=' > reg
    
		
	outmesh=$(ls -d1v */ | /bin/grep -v 'constant\|plots\|system' | wc -l)
	if [[ "$outmesh" > 1 ]]; then
		result=$(ls -d1v */ | /bin/grep -v 'constant\|plots\|system' | cut -f1 -d'/' | tail -1)
		echo 'Iterations = '$result
	else
		result=0
		echo 'Iterations did not converge'
	fi
	
	if [[ "$result" != "0" ]]; then
		gmtFoam -region rMesh -time $result mesh > /dev/null
	fi
    
    if [[ -d "3" ]]; then
        gmtFoam -region rMesh -time 3 mesh > /dev/null
    fi
    
    
    cd -
done
make_scaldata.sh
cd $here



exit 0

