#!/bin/bash
set -o nounset
set -o errexit
set -o verbose

#methods=(FP2D PMA2D)
methods=(PMA2D)
monitors=('ring')

resultspath=/home/hilary/OpenFOAM/hilary-dev/run/MongeAmpere/results
scriptpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere/scripts
runpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere
al2drunpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere
fp2drunpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere
pma2drunpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere

alconv='1.0e-8'
alN=(60)
algamma=(1.0)

fpconv='1.0e-8'
fpN=(60)
fpgamma=(0.80 0.85 0.90 0.95 1.00 1.05 1.10 1.15)

pmaconv='1.0e-8'
pmaN=(60)
pmagamma=(0.45 0.50 0.55 0.60 0.65 0.70 0.75)
pmadt=(0.15 0.20 0.25 0.30)


remove=gvfs-trash

set_mon(){
    if [[ "$1" == "bell" ]]; then
	$scriptpath/ofmonitor.sh -b > /dev/null
    elif [[ "$1" == "ring" ]]; then
	$scriptpath/ofmonitor.sh -r > /dev/null
    else
	echo 'Error: wrong monitor function selected'
	exit -1
    fi
}    


for method in "${methods[@]}"; do
    if [[ "$method" == "AL2D" ]]; then
	cd $al2drunpath
	pwd
	$scriptpath/ofal.sh -c $alconv
	
	#loop over sizes
	for N in "${alN[@]}"; do
	    $scriptpath/ofal.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		
		set_mon $mon
		
		#loop over parameters (none in AL)
		#loop over parameters (only gamma in AL now)
		for gamma in "${algamma[@]}"; do
		    $scriptpath/ofal.sh -g $gamma > /dev/null
		    echo $method $N $mon $gamma


		    AL2D > log 2> err
		    grep PABe log | cut -d= -f3 > equi
		    grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
		    
		    resulttarget=$resultspath/$mon/$method/$N/$gamma/
		    mkdir -p $resulttarget
		    
		    #result=$(( $(wc -l equi | cut -f1 -d' ' )-1 ))
		    outmesh=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system\|scripts' | wc -l)
		    if [[ "$outmesh" > 1 ]]; then
			result=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system\|scripts' | cut -f1 -d'/' | tail -n +2)
			echo 'Iterations = '$result
		    else
			result=0
			echo 'Iterations did not converge'
		    fi
		    
		    if [[ "$result" != "0" ]]; then
			    echo gmtFoam -region rMesh -time $result mesh > /dev/null
			    echo mv $result/mesh.pdf $resulttarget
			    $remove $result
		    fi
		    
		    mv log err equi time $resulttarget
		    cp system/{fvSchemes,fvSolution,controlDict,AL2DDict} $resulttarget
		    cp system/blockMeshDict $resulttarget
		done
	    done
	    
	done

    elif [[ "$method" == "FP2D" ]]; then

	cd $fp2drunpath
	pwd
	$scriptpath/offp.sh -c $fpconv
	
	#loop over sizes
	for N in "${fpN[@]}"; do
	    $scriptpath/offp.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		
		set_mon $mon
		
		#loop over parameters (only gamma in FP)
		for gamma in "${fpgamma[@]}"; do
		    $scriptpath/offp.sh -g $gamma > /dev/null
		    echo $method $N $mon $gamma
		    FP2D > log 2> err
		    grep PABe log | cut -d= -f3 > equi
		    grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
		    
		    resulttarget=$resultspath/$mon/$method/$N/$gamma/
		    mkdir -p $resulttarget
		    outmesh=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system\|scripts' | wc -l)
		    if [[ "$outmesh" > 1 ]]; then
			result=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system\|scripts' | cut -f1 -d'/' | tail -n +2)
			echo 'Iterations = '$result
		    else
			result=0
			echo 'Iterations did not converge'
		    fi

		    if [[ "$result" != "0" ]]; then
			    echo gmtFoam -region rMesh -time $result mesh > /dev/null
			    echo mv $result/mesh.pdf $resulttarget
			    $remove $result
		    fi

		    mv log err equi time $resulttarget
		    cp system/{fvSchemes,fvSolution,controlDict,FP2DDict} $resulttarget
		    cp system/blockMeshDict $resulttarget
		    #if [[ "$result" != "0" ]]; then
			#    $remove $result
		    #fi
		done
	    done
	    
	done


    elif [[ "$method" == "PMA2D" ]]; then

	cd $pma2drunpath
	pwd
	$scriptpath/ofpma.sh -c $fpconv
	
	#loop over sizes
	for N in "${pmaN[@]}"; do
	    $scriptpath/ofpma.sh -N $N
	    
	    #loop over monitors
	    for mon in "${monitors[@]}"; do
		
		set_mon $mon
				
		#loop over parameters (none in AL)
		for gamma in "${pmagamma[@]}"; do
		    for dt in "${pmadt[@]}"; do
			$scriptpath/ofpma.sh -g $gamma -d $dt
			echo $method $N $mon $gamma $dt
			PMA2D > log 2> err
			grep PABe log | cut -d= -f3 > equi
			grep ExecutionTime log | cut -d= -f2 | cut -ds -f1 > time
			resulttarget=$resultspath/$mon/$method/$N/$gamma/$dt/
			mkdir -p $resulttarget
			time=$(cat time)
			if [[ "$time" != "" ]]; then
			    result=$(ls -d1 */ | /bin/grep -v 'constant\|plots\|system\|scripts' | cut -f1 -d'/' | tail -n +2)
			    echo $result
			    echo gmtFoam -region rMesh -time $result mesh
			    echo mv $result/mesh.pdf $resulttarget
			    if [[ "$result" != "0" ]]; then
                    $remove $result
			    fi
			fi

			mv log err equi time $resulttarget
			cp system/{fvSchemes,fvSolution,controlDict,PMA2DDict} $resulttarget
			cp system/blockMeshDict $resulttarget

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
