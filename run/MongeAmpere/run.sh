blockMesh && mkdir -p constant/rMesh && cp -r constant/polyMesh constant/rMesh

#Then run the appropriate executable:
AFP
FP2D
PMA2D
Newton2d-vectorGradc_m_reconstruct
Newton2d-vectorGradc_m_exact

# Post-processing
time=???
gmtFoam -time $time mesh -region rMesh
gv $time/mesh.pdf &

