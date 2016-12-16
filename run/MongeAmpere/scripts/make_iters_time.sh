#!/bin/bash
set -o nounset
#set -o errexit
set -o verbose


file='/home/pbrowne/OpenFOAM/results/iters_time.tex'
table='/home/pbrowne/OpenFOAM/results/iters_time.table'

cat <<EOF > $file
\centering\begin{tikzpicture}
\begin{loglogaxis}[%xmin=0,ymin=0,
width=10cm,height=8cm,
ytick={2,3,4,5,6,7,8,9,10,20,30,40},
yticklabels={,3,,,,,,,10,20,30,40},
%legend style={at={(1.01,0.5)},anchor=west,font=\tiny},
legend pos=north west,legend style={font=\tiny},
legend columns=2,xlabel=Iterations,ylabel=Wall-clock time (seconds),
enlargelimits=true,
scatter/classes={%
AFP_bell={mark=text,text mark=$\bell$,black},% 
NEWTON_bell={mark=text,text mark=$\bell$,Dark2-8-1},% 
PMA_bell={mark=text,text mark=$\bell$,Dark2-8-3},% 
FP_bell={mark=text,text mark=$\bell$,Dark2-8-2},
AFP_ring={mark=o,black},% 
NEWTON_ring={mark=o,Dark2-8-1},% 
PMA_ring={mark=o,Dark2-8-3},% 
FP_ring={mark=o,Dark2-8-2}}]
\addplot[scatter,only marks,mark options={line width=2pt},
scatter src=explicit symbolic]%
table[meta=label] {$table};
\legend{AFP Bell,Newton Bell,PMA Bell,FP Bell,
AFP Ring,Newton Ring,PMA Ring,FP Ring}
\end{loglogaxis}
\end{tikzpicture}
EOF

cat <<EOF > $table
x y label
EOF

hm=$(pwd)
#start with AFP:
cd /home/pbrowne/OpenFOAM/results/bell/AFP/60
iters=$( wc -l equi | cut -f1 -d' ')
let iters=$iters-1
echo $iters $(cat time) AFP_bell >> $table

cd /home/pbrowne/OpenFOAM/results/bell/Newton2d-vectorGradc_m_reconstruct/60
iters=$( wc -l equi | cut -f1 -d' ')
let iters=$iters-1
echo $iters $(cat time) NEWTON_bell >> $table

cd $hm
cd /home/pbrowne/OpenFOAM/results/bell/FP2D/60
for dir in */; do
    cd $dir
    iters=$( wc -l equi | cut -f1 -d' ')
    let iters=$iters-1
    echo $iters $(cat time) FP_bell >> $table
    cd -
done
cd $hm
cd /home/pbrowne/OpenFOAM/results/bell/PMA2D/60
for dir in *; do
#for dir in 0.45 0.50 0.55 0.60 0.65 0.70 0.75 0.80; do
    echo $dir
    cd $dir
    for dir2 in */; do
        cd $dir2
        echo $dir2
        iters=$( wc -l equi | cut -f1 -d' ')
        let iters=$iters-1
        if [[ -f mesh.pdf ]]; then 
            echo $iters $(cat time) PMA_bell >> $table
        fi
        cd ..
    done
    cd ..
    pwd
done
cd $hm


hm=$(pwd)
#start with AFP:
cd /home/pbrowne/OpenFOAM/results/ring/AFP/60
iters=$( wc -l equi | cut -f1 -d' ')
let iters=$iters-1
echo $iters $(cat time) AFP_ring >> $table

cd /home/pbrowne/OpenFOAM/results/ring/Newton2d-vectorGradc_m_reconstruct/60
iters=$( wc -l equi | cut -f1 -d' ')
let iters=$iters-1
echo $iters $(cat time) NEWTON_ring >> $table

cd $hm
cd /home/pbrowne/OpenFOAM/results/ring/FP2D/60
for dir in */; do
    cd $dir
    iters=$( wc -l equi | cut -f1 -d' ')
    let iters=$iters-1
    echo $iters $(cat time) FP_ring >> $table
    cd -
done
cd $hm
cd /home/pbrowne/OpenFOAM/results/ring/PMA2D/60
for dir in *; do
#for dir in 0.45 0.50 0.55 0.60 0.65 0.70 0.75 0.80; do
    echo $dir
    cd $dir
    for dir2 in */; do
        cd $dir2
        echo $dir2
        iters=$( wc -l equi | cut -f1 -d' ')
        let iters=$iters-1
        if [[ -f mesh.pdf ]]; then 
            echo $iters $(cat time) PMA_ring >> $table
        fi
        cd ..
    done
    cd ..
    pwd
done
cd $hm
