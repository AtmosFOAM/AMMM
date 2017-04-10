#!/bin/bash
set -o nounset
#set -o errexit


methods=(AL2D)
monitors=('ring')

resultspath=/home/pbrowne/OpenFOAM/results
 al2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/AL2D
 fp2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/FP2D
pma2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/PMA2D

alconv='1.0e-8'
alN=(60)
algamma=(1.0)

fpconv='1.0e-8'
fpN=(60)
fpgamma=(2.5 2.55 2.6 2.65 2.7 2.75 2.8 2.85 2.9 2.95 3.0 3.05 3.1 3.15)

pmaconv='1.0e-8'
pmaN=(60)
pmagamma=(0.45 0.5)
pmadt=(0.1 0.15 0.2 0.25)




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
			rm -rf $result
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
			rm -rf $result
		    fi

		    mv log err equi time $resulttarget
		    cp system/{fvSchemes,fvSolution,controlDict,FP2DDict} $resulttarget
		    cp constant/polyMesh/blockMeshDict $resulttarget
		    if [[ "$result" != "0" ]]; then
			rm -rf $result
		    fi
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
				rm -rf $result
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
