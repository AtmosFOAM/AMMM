# Run all advectionOT test cases at four resolutions and calculate l2 norms

# run all cases at four resolutions
for dir in fixedNoMountain fixedOverMountains \
           movingNoMountain movingOverMountains movingOverMountainsA; do
    cd $dir
    for case in n50 n100 n200 n400; do
        cd $case
        ./run.sh
        cd ..
    done
    cd ..
done

# Calculate norms
for dir in fixedNoMountain fixedOverMountains \
           movingNoMountain movingOverMountains movingOverMountainsA; do
    cd $dir
    for case in n50 n100 n200 n400; do
        cd $case
        ./convergence.sh
        cd ..
    done
    cd ..
done

# Create graphs
echo '#meshSize gridSize 1stOrder 2ndOrder fixedNoMountain fixedOverMountains movingNoMountain movingOverMountainsA' > convergence.dat
echo -e "50\n100\n200\n400\n" > meshSize.dat
echo -e "200\n100\n50\n25\n" > gridSize.dat
echo -e "0.04\n0.02\n0.01\n0.005\n" > 1stOrder.dat
echo -e "0.04\n0.01\n0.0025\n0.000625\n" > 2ndOrder.dat

for dir in fixedNoMountain fixedOverMountains \
           movingNoMountain movingOverMountainsA; do
    cd $dir
    for case in n50 n100 n200 n400; do
        cd $case
        grep '\b600 ' globalSumTerror.dat | awk '{print $3}' >> ../../$dir.dat
        cd ..
    done
    cd ..
done
paste meshSize.dat gridSize.dat 1stOrder.dat 2ndOrder.dat fixedNoMountain.dat fixedOverMountains.dat movingNoMountain.dat movingOverMountainsA.dat>> convergence.dat
rm meshSize.dat gridSize.dat 1stOrder.dat 2ndOrder.dat fixedNoMountain.dat fixedOverMountains.dat movingNoMountain.dat movingOverMountainsA.dat
gmtPlot plots/l2norms.gmt
