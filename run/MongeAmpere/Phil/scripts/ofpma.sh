#!/bin/bash
set -o nounset
set -o errexit

usage(){
    me=$(basename "$0")
    echo Usage of $me:
    echo 
    echo -e "\t $me is designed to change the parameters of PMA to solve the"
    echo -e "\t Monge-Ampere equation using OpenFOAM PMA2D"
    echo
    echo -e "\t OPTIONS:"
    echo -e "\t \t -N N \t Use a grid of NxN cells" 
    echo -e "\t \t -X N \t Use N cells in the X direction"
    echo -e "\t \t \t MUST BE USED IN COMBINATION WITH -Y"
    echo -e "\t \t -Y N \t Use N cells in the Y direction"
    echo -e "\t \t \t MUST BE USED IN COMBINATION WITH -X"
    echo -e "\t \t -d dt \t Set the timestep to dt"
    echo -e "\t \t -g gamma \t Set the smoothing parameter gamma"
    echo -e "\t \t -c conv \t Set the convergence criteria to conv"
    echo -e "\t \t -h \t \t Display this help text"
}

scriptpath=/home/hilary/OpenFOAM/hilary-dev/AMMM/run/MongeAmpere/scripts
nval=False
xval=False
yval=False
dval=False
gval=False
cval=False
mesh=system/blockMeshDict
rmesh=system/blockMeshDict
dict=system/PMA2DDict
controldict=system/controlDict

while getopts ":N:X:Y:d:g:c:h" opt; do
    case $opt in
	g)
	    gval=True
	    gamma=$OPTARG
	    ;;
	d)
	    dval=True
	    dt=$OPTARG
	    ;;
	c)
	    cval=True
	    conv=$OPTARG
	    ;;
	N)
	    nval=True
	    N=$OPTARG
	    X=$N
	    Y=$N
	    ;;
	X)
	    xval=True
	    X=$OPTARG
	    ;;
	Y)
	    yval=True
	    Y=$OPTARG
	    ;;
	h)
	    usage
	    exit 0
	    ;;
	\?)
	    echo "Invalid option: -$OPTARG" >&2
	    usage
	    exit 1
	    ;;
	:)
	    echo "Option -$OPTARG requires an argument." >&2
	    usage
	    exit 1
	    ;;
    esac
done


if [[ "$xval" != "$yval" ]]; then
    echo "ERROR: Both X and Y must be specified"
    usage
    exit -1
fi

if [[ "$nval" == "True" || "$xval" == "True" || "$yval" == "True" ]]; then
    oldres=$(/bin/grep simpleGrading $mesh | cut -d'(' -f3 | cut -d')' -f 1)
    sed -i "s/$oldres/$X $Y 1/" $mesh
    sed -i "s/$oldres/$X $Y 1/" $rmesh
    $scriptpath/ofmesh.sh
fi


if [[ "$dval" == "True" ]]; then
    sed -i "s/^deltaT.*/deltaT          $dt;/" $controldict
    #wi=$( echo "50 * $dt" | bc -l )
    #sed -i "s/^writeInterval.*/writeInterval   $wi;/" $controldict
fi

if [[ "$gval" == "True" ]]; then
    sed -i "s/^Gamma.*/Gamma Gamma [0 0 1 0 0] $gamma;/" $dict
fi

if [[ "$cval" == "True" ]]; then
    sed -i "s/^conv.*/conv $conv;/" $dict
fi
