#!/bin/bash
set -o nounset
set -o errexit
set -o verbose 
set -o xtrace

methods=(AFP)
monitors=('bell' 'ring')

resultspath=results
bl2drunpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere/MongeAmpere/BL2D
AFPpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere/AFP

mkdir -p $resultspath/{bell,ring}/AFP

blconv='1.0e-8'
blN=(50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220 230 240 250 260 270 280 290 300)
blN=(50 60 70 80 90 100 110 120 140 160 180 200 250 300) # 400 500)
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




for mon in "${monitors[@]}"; do
    cd $resultspath
    cd $mon
    cd AFP
    for N in "${blN[@]}"; do
        mkdir -p $N
        cd $N
        cp -r $AFPpath/{0,constant,system} .

        ofafp.sh -N $N

        if [[ "$mon" == "ring" ]]; then
            ofmonitor.sh -r
        elif [[ "$mon" == "bell" ]] ; then
            ofmonitor.sh -b
        else
            echo 'wrong monitor function selected'
            exit -1
        fi

        AFP > log 2> err
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
		    echo gmtFoam -region rMesh -time $result mesh > /dev/null
	    fi
        
        if [[ -d "3" ]]; then
            echo gmtFoam -region rMesh -time 3 mesh > /dev/null
        fi
        cd -
    done #N
done #mon


    
wd=$(pwd)

#ring:
cd /home/pbrowne/OpenFOAM/results/ring/AFP
for N in */; do
    cd "$N"
    rm -rf regs
    touch regs
    while read line; do
        echo $line
        let a=$line+1
        echo $line $(sed -n "$a"p equi) >> regs
    done < reg
    cd -
done

#bell:
cd /home/pbrowne/OpenFOAM/results/bell/AFP
for N in */; do
    cd "$N"
    rm -rf regs
    touch regs
    while read line; do
        echo $line
        let a=$line+1
        echo $line $(sed -n "$a"p equi) >> regs
    done < reg
    cd -
done


