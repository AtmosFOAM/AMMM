# clean up first
rm -r constant/polyMesh [1-9]* constant/rMesh

blockMesh && mkdir -p constant/rMesh && cp -r constant/polyMesh constant/rMesh

#Then run the appropriate executable:
AFP                 # 53 iterations
FP2D                # generates error message
PMA2D               # tangles and gives error messages
Newton2d-vectorGradc_m_reconstruct  # 46 iterations
Newton2d-vectorGradc_m_exact        # error message

# Post-processing
time=???
gmtFoam -time $time mesh -region rMesh
gv $time/mesh.pdf &

