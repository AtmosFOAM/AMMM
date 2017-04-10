#!/bin/bash
set -o nounset
set -o errexit
set -o verbose 
set -o xtrace

methods=(BL2D)
monitors=('bell' 'ring')

resultspath=/home/pbrowne/OpenFOAM/results
 bl2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/BL2D

blconv='1.0e-8'
blN=(50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220 230 240 250 260 270 280 290 300)
blN=(50 60 70 80 90 100 110 120 140 160 180 200 250 300 400 500)
blgamma=(1.0)

remove=gvfs-trash


set_mon(){
    if [[ "$1" == "bell" ]]; then
	ofmonitor.sh -b > /dev/null
    elif [[ "$1" == "ring" ]]; then
	ofmonitor.sh -r > /dev/null
    else
	echo 'Error: wrong monitor function selected'
	exit -1
    fi
}    


for method in "${methods[@]}"; do
    if [[ "$method" == "BL2D" ]]; then
	cd $bl2drunpath
	pwd
	ofbl.sh -c $blconv
	
	#loop over sizes
	for N in "${blN[@]}"; do
	    ofbl.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		    
		    set_mon $mon
		    
		    #loop over parameters (none in AL)
		    #loop over parameters (only gamma in AL now)
		    for gamma in "${blgamma[@]}"; do
		        ofbl.sh -g $gamma > /dev/null
		        echo $method $N $mon $gamma
                
                
		        BL2D > log 2> err
		        grep PABe log | cut -d= -f3 > equi
		        grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
                grep -A 1 'Time =' log | grep -B 1 Minimum | grep Time | cut -f2 -d'=' > reg

		        resulttarget=$resultspath/$mon/$method/$N/$gamma/
		        mkdir -p $resulttarget
		        
		        #result=$(( $(wc -l equi | cut -f1 -d' ' )-1 ))
		        outmesh=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system' | wc -l)
		        if [[ "$outmesh" > 1 ]]; then
			        result=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system' | cut -f1 -d'/' | tail -n +2)
			        echo 'Iterations = '$result
		        else
			        result=0
			        echo 'Iterations did not converge'
		        fi
		        
		        if [[ "$result" != "0" ]]; then
			        gmtFoam -region rMesh -time $result mesh > /dev/null
			        mv $result/mesh.pdf $resulttarget
			        $remove $result
		        fi
		        
		        mv log err equi time reg $resulttarget
		        cp system/{fvSchemes,fvSolution,controlDict,BL2DDict} $resulttarget
		        cp constant/polyMesh/blockMeshDict $resulttarget
		    done
	    done

    done	    
    else
	    echo Wrong method, $method, selected in $(basename "$0")
    fi
done
    


do_regs.sh
