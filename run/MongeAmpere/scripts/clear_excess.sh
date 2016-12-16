#!/bin/bash

echo "code to remove extra files made by test runs"
echo "specifically - time folders that are not the final or starting ones"

for mon in bell ring; do
    path=results/$mon
    #echo $path
    for method in NEWTON2D-pab; do
        path=$path"/$method"
        echo $path
        for N in $path/*/; do
            echo $N
            for type in "$N"*/; do
                echo $type
                constant="$type"constant/
                system="$type"system/
                echo $constant
                ls -vd "$type"*/
                echo $constant
                echo $system
            done
        done
    done
done
