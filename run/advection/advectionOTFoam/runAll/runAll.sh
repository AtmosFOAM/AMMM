#!/bin/bash -ev
#
# Reproduce all the results for the paper

# Run all the test cases to calculate the convergence rate
./scripts/convergenceTest.sh
# Plot the results for test cases at the default resolution
./scripts/plotAll.sh
# Run & plot the results for longer test cases
./scripts/longRun.sh
# Plot mesh convergence
./scripts/plotMeshConvergence.sh
# Collect figures for the paper
./scripts/collectFigures.sh
