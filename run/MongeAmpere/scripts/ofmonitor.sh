#!/bin/bash
set -o nounset
set -o errexit

usage(){
    me=$(basename "$0")
    echo Usage of $me:
    echo 
    echo -e "\t $me is designed to change the monitor function in the"
    echo -e "\t Monge-Ampere equation"
    echo
    echo -e "\t OPTIONS:"
    echo -e "\t \t -b \t \t Use the Bell monitor function"
    echo -e "\t \t -r \t \t Use the Ring monitor function"
    echo -e "\t \t -h \t \t Display this help text"
}

bval=False
rval=False





while getopts "brh" opt; do
    case $opt in
	b)
	    bval=True
	    ;;
	r)
	    rval=True
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










exists=False
for method in {Newton2d-vectorGradc_m_reconstruct,AFP,NEWTON2D-vectorGradc_m,PMA2D,FP2D,AL2D,NEWTON2D,BL2D,NEWTON2D-pab}; do
    #    echo $method
    dictcheck='system/'$method'Dict'
    #    echo $dictcheck
    if [[ -f "$dictcheck" ]]; then
	    echo file $dictcheck exists
	    dict=$dictcheck
	    exists=True

        if [[ "$bval" == "True" ]]; then
            a1=50
            a2=100
            a=0.0
            sed -i 's|^alpha1|//alpha1|g' $dict
            sed -i 's|^alpha2|//alpha2|g' $dict
            sed -i 's|^a |//a |g' $dict
            sed -i "s|//alpha1 $a1;|alpha1 $a1;|" $dict
            sed -i "s|//alpha2 $a2;|alpha2 $a2;|" $dict
            sed -i "s|//a $a;|a $a;|" $dict
            cat $dict
        fi
        if [[ "$rval" == "True" ]]; then
            a1=10
            a2=200
            a=0.25
            sed -i 's|^alpha1|//alpha1|g' $dict
            sed -i 's|^alpha2|//alpha2|g' $dict
            sed -i 's|^a |//a |g' $dict
            sed -i "s|//alpha1 $a1;|alpha1 $a1;|" $dict
            sed -i "s|//alpha2 $a2;|alpha2 $a2;|" $dict
            sed -i "s|//a $a;|a $a;|" $dict
            cat $dict
        fi

    fi
    
done

if [[ "$exists" == "False" ]]; then
    echo No OpenFOAM application directory can be found in which to set the monitor function
    exit -1
fi





#done
