# Create graphs
mkdir -p plots
gmtPlot diagnostics/Vchange.gmt
gmtPlot diagnostics/Tchange.gmt
gmtPlot diagnostics/ATchange.gmt
grep '\bTime =' log | awk '{print $3}' > time.dat
grep '\bT goes from' log | awk '{print $4, $6}' > T.dat
grep '\buniT goes from' log | awk '{print $4, $6}' > uniT.dat
grep '\bA goes from ' log | awk '{print $4, $6}' > A.dat
echo '#time Tmin Tmax uniTmin uniTmax Amin Amax' > minMax.dat
paste time.dat T.dat uniT.dat A.dat >> minMax.dat
rm time.dat T.dat uniT.dat A.dat
gmtPlot diagnostics/uniT.gmt
gmtPlot diagnostics/A.gmt

