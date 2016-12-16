#!/bin/bash
set -o nounset
set -o errexit
set -o verbose

methods=(FP2D)
monitors=('bell')

resultspath=/home/pbrowne/OpenFOAM/results
 al2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/run
 fp2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/run
pma2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/run

alconv='1.0e-8'
alN=(60)
algamma=(1.0)

fpconv='1.0e-8'
fpN=(60 80 100 120 140 160 180 200 250 300)
fpN=(300 250 200 180 160 140 120 100 80)
fpN=(160 140 120 100 80)
fpN=(300)
fpgamma=(0.80 0.90 1.00 1.10 1.20)
fpgamma=(3.10 3.00 2.90 2.80 2.70 2.60)
fpgamma=(3.10 3.00)

pmaconv='1.0e-8'
pmaN=(80 100 120 140 160 180 200 250 300)
pmaN=(300 250 200 180 160 140 120 100 80)
pmagamma=(0.50 0.60 0.70)
pmadt=(0.20 0.25 0.30)


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
    if [[ "$method" == "AL2D" ]]; then
	cd $al2drunpath
	pwd
	ofal.sh -c $alconv
	
	#loop over sizes
	for N in "${alN[@]}"; do
	    ofal.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		
		set_mon $mon
		
		#loop over parameters (none in AL)
		#loop over parameters (only gamma in AL now)
		for gamma in "${algamma[@]}"; do
		    ofal.sh -g $gamma > /dev/null
		    echo $method $N $mon $gamma


		    AL2D > log 2> err
		    grep PABe log | cut -d= -f3 > equi
		    grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
		    
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
		    
		    mv log err equi time $resulttarget
		    cp system/{fvSchemes,fvSolution,controlDict,AL2DDict} $resulttarget
		    cp constant/polyMesh/blockMeshDict $resulttarget
		done
	    done
	    
	done

    elif [[ "$method" == "FP2D" ]]; then

	cd $fp2drunpath
	pwd
	offp.sh -c $fpconv
	
	#loop over sizes
	for N in "${fpN[@]}"; do
	    offp.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		
		set_mon $mon
		
		#loop over parameters (only gamma in FP)
		for gamma in "${fpgamma[@]}"; do
		    offp.sh -g $gamma > /dev/null
		    echo $method $N $mon $gamma
		    FP2D > log 2> err
		    grep PABe log | cut -d= -f3 > equi
		    grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
		    
		    resulttarget=$resultspath/$mon/$method/$N/$gamma/
		    mkdir -p $resulttarget
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

		    mv log err equi time $resulttarget
		    cp system/{fvSchemes,fvSolution,controlDict,FP2DDict} $resulttarget
		    cp constant/polyMesh/blockMeshDict $resulttarget
		    #if [[ "$result" != "0" ]]; then
			#    $remove $result
		    #fi
		done
	    done
	    
	done


    elif [[ "$method" == "PMA2D" ]]; then

	cd $pma2drunpath
	pwd
	ofpma.sh -c $fpconv
	
	#loop over sizes
	for N in "${pmaN[@]}"; do
	    ofpma.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		
		set_mon $mon
				
		#loop over parameters (none in AL)
		for gamma in "${pmagamma[@]}"; do
		    for dt in "${pmadt[@]}"; do
			ofpma.sh -g $gamma -d $dt
			echo $method $N $mon $gamma $dt
			PMA2D > log 2> err
			grep PABe log | cut -d= -f3 > equi
			grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
			resulttarget=$resultspath/$mon/$method/$N/$gamma/$dt/
			mkdir -p $resulttarget
			time=$(cat time)
			if [[ "$time" != "" ]]; then
			    result=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system' | cut -f1 -d'/' | tail -n +2)
			    echo $result
			    gmtFoam -region rMesh -time $result mesh
			    mv $result/mesh.pdf $resulttarget
			    if [[ "$result" != "0" ]]; then
                    $remove $result
			    fi
			fi

			mv log err equi time $resulttarget
			cp system/{fvSchemes,fvSolution,controlDict,PMA2DDict} $resulttarget
			cp constant/polyMesh/blockMeshDict $resulttarget

		    done
		done
		#end loop over parameters

	    done
	    #end loop over monitors

	done
	#end loop over sizes

    else
	echo Wrong method, $method, selected in $(basename "$0")
    fi
done
