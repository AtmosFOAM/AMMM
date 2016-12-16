#!/bin/bash -e
#
./setup.sh
movingIcoFoamA >& log &
tail -f log

