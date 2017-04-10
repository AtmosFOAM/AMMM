#!/bin/bash
set -o nounset
#set -o errexit
set -o verbose 
set -o xtrace

methods=(NEWTON2D-pab)
monitors=('bell' 'ring')

resultspath=/home/pbrowne/OpenFOAM/results
 bl2drunpath=/home/pbrowne/OpenFOAM/pbrowne-3.0.1/AtmosFOAM-3.0.1/run/MongeAmpere/run/

blconv='1.0e-8'
blN=(60)
NewtonN=(200)
blgamma=(1.0)

remove=gvfs-trash


set_mon(){
    dict=system/NEWTON2D-pabDict
    
    if [[ "$1" == "bell" ]]; then


    a1=50
    a2=100
    a=0.0
    sed -i 's|^alpha1|//alpha1|g' $dict
    sed -i 's|^alpha2|//alpha2|g' $dict
    sed -i 's|^a |//a |g' $dict
    sed -i "s|//alpha1 $a1;|alpha1 $a1;|" $dict
    sed -i "s|//alpha2 $a2;|alpha2 $a2;|" $dict
    sed -i "s|//a $a;|a $a;|" $dict


    elif [[ "$1" == "ring" ]]; then
    a1=10
    a2=200
    a=0.25
    sed -i 's|^alpha1|//alpha1|g' $dict
    sed -i 's|^alpha2|//alpha2|g' $dict
    sed -i 's|^a |//a |g' $dict
    sed -i "s|//alpha1 $a1;|alpha1 $a1;|" $dict
    sed -i "s|//alpha2 $a2;|alpha2 $a2;|" $dict
    sed -i "s|//a $a;|a $a;|" $dict

    else
	echo 'Error: wrong monitor function selected'
	exit -1
    fi

}    


for mon in "${monitors[@]}"; do
    home=$resultspath/$mon/NEWTON2D-pab/
    mkdir -p $home
    cd $home
    
    for N in "${NewtonN[@]}"; do
        mkdir -p $N
        cd $N

    for diff in linear upwind downwind; do
        mkdir -p $diff
        cd $diff
        cp -r $bl2drunpath/{0,constant,system} .
        ofbl.sh -N $N
        
        cat << EOF > system/fvSolution
/*--------------------------------*- C++ -*----------------------------------*\
| =========                 |                                                 |
| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |
|  \\    /   O peration     | Version:  1.5                                   |
|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |
|    \\/     M anipulation  |                                                 |
\*---------------------------------------------------------------------------*/
FoamFile
{
    version     2.0;
    format      ascii;
    class       dictionary;
    object      fvSolution;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

solvers
{
    "Phi|phi"
    {
        solver GAMG;
        smoother symGaussSeidel;
        preconditioner   DILU;
        tolerance        1e-12;
        relTol           1e-8;
        maxIter          500;
    };
}

relaxationFactors
{
    equations
    {
        Phi               0.999;
        phi               0.999;
    }
}
// ************************************************************************* //

EOF

        cat << EOF > system/fvSchemes
/*---------------------------------------------------------------------------*\
| =========                 |                                                 |
| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |
|  \\    /   O peration     | Version:  1.4                                   |
|   \\  /    A nd           | Web:      http://www.openfoam.org               |
|    \\/     M anipulation  |                                                 |
\*---------------------------------------------------------------------------*/

FoamFile
{
    version         2.0;
    format          ascii;

    root            "";
    case            "";
    instance        "";
    local           "";

    class           dictionary;
    object          fvSchemes;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

ddtSchemes
{
    default Euler;
}

gradSchemes
{
    default         none;
    grad(phiBar)    Gauss linear;
    grad(phi)       Gauss linear;
    grad(Phi)       Gauss linear;
}

divSchemes
{
    default             none;
    div((magSf*sngradc_m),phi) Gauss ;
    div(sngradc_m,phi) Gauss ;
}

laplacianSchemes
{
    default     Gauss linear uncorrected;
    laplacian(Phi) Gauss linear uncorrected;
    laplacian(phiBar) Gauss linear uncorrected;
    laplacian(c_m) Gauss linear uncorrected;
    laplacian(matrixA,Psi) Gauss linear uncorrected;
    laplacian(matrixA,Phi) Gauss linear uncorrected;
    laplacian(sqr((1|deltaCoeffs)),monitorNew) Gauss linear uncorrected;
}

interpolationSchemes
{
    default        linear;
    interpolate(gradPhi) linear;
    interpolate(gradPhi) linear;
    interpolate(grad(Psi))  linear;
    interpolate(grad(Phi))  linear;
}

snGradSchemes
{
    default         none;
    snGrad(phiBar)     uncorrected;
    snGrad(Phi)     uncorrected;
    snGrad(c_m) uncorrected;
    snGrad(((domainIntegrate(detHess)|domainIntegrate((1|monitorNew)))|monitorR)) uncorrected;
}

fluxRequired
{
    default         no;
}


// ************************************************************************* //
EOF


        cat <<EOF > system/NEWTON2D-pabDict
// The FOAM Project // File: fvSchemes
/*
-------------------------------------------------------------------------------
 =========         | dictionary
 \\      /         |
  \\    /          | Name:   fvSchemes
   \\  /           | Family: FoamX configuration file
    \\/            |
    F ield         | FOAM version: 2.3
    O peration     | Product of Nabla Ltd.
    A and          |
    M anipulation  | Email: Enquiries@Nabla.co.uk
-------------------------------------------------------------------------------
*/
// FoamX Case Dictionary.

FoamFile
{
    version         2.0;
    format          ascii;

    root            "";
    case            "";
    instance        "system";
    local           "";

    class           dictionary;
    object          AL2DDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

monitorFunctionFrom monitorFunctionSech;

// Ring
alpha1 10;
alpha2 200;
a 0.25;

// Bell
//alpha1 50;
//alpha2 100;
//a 0.0;

centre (0 0 0);
Gamma1 Gamma1 [0 0 0 0 0] 1;
Gamma2 Gamma2 [0 0 0 0 0] 1;
conv 1.0e-8;

// ************************************************************************* //
EOF
        
        set_mon $mon

        #set the schemes:
        sed -i "s/div((magSf\*sngradc_m),phi) Gauss.*/div((magSf*sngradc_m),phi) Gauss $diff;/g" system/fvSchemes
        sed -i "s/div(sngradc_m,phi) Gauss.*/div(sngradc_m,phi) Gauss $diff;/g" system/fvSchemes
        sed -i "s/writeInterval .*/writeInterval   3\;/g" system/controlDict


        NEWTON2D-pab > log 2> err
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

    cd $home
    done
    make_scaldata.sh
done







exit 0

