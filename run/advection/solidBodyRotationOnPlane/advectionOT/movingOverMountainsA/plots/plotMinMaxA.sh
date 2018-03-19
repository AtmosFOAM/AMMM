# Plot the minimum and maximum values of A (found from log file)
cp log logTmp
grep 'A goes from' logTmp | awk '{print $4, $6}' > AminmaxTmp.dat
grep 'Time =' logTmp | awk '{if ($1=="Time") { print $3}}' > timeTmp.dat
paste timeTmp.dat AminmaxTmp.dat | awk '{if (NF==3){print $1,$2,$3}}' > Aminmax.dat
rm AminmaxTmp.dat timeTmp.dat logTmp
gmtPlot plots/plotMinMaxA.gmt

