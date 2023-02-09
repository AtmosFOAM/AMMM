# Create graphs
mkdir -p plots
gmtPlot diagnostics/AVchange.gmt
grep '\bTime =' log | awk '{print $3}' > time.dat
grep '\bT goes from' log | awk '{print $4, $6}' > T.dat
grep '\buniT goes from' log | awk '{print $4, $6}' > uniT.dat
grep '\bA goes from ' log | awk '{print $4, $6}' > A.dat
grep '\bVariance of A' log | awk '{print $5}' > Avar.dat
echo '#time Tmin Tmax uniTmin uniTmax Amin Amax Avar' > minMax.dat
paste time.dat T.dat uniT.dat A.dat Avar.dat >> minMax.dat
rm time.dat T.dat uniT.dat A.dat Avar.dat
#gmtPlot diagnostics/uniT.gmt
gmtPlot diagnostics/A.gmt
gmtPlot diagnostics/Avar.gmt

