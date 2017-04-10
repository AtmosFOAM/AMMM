#!/bin/bash
set -o nounset
set -o errexit

usage(){
    me=$(basename "$0")
    echo Usage of $me:
    echo 
    echo -e "\t $me is designed to run the code to initialise the meshes for the"
    echo -e "\t Monge-Ampere equation to be used by OpenFOAM"
    echo
    echo -e "\t OPTIONS:"
    echo -e "\t \t -h \t \t Display this help text"
}


while getopts "h" opt; do
    case $opt in
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


# generate the mesh
blockMesh > /dev/null
# copy the mesh to the re-distributed mesh
mkdir -p constant/rMesh
cp -r constant/polyMesh constant/rMesh
