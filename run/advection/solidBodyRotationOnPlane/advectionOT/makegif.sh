#!/bin/bash -e

for field in mesh A T uniT monitor meshOverMountain; do
    eps2gif $field.gif ?/$field.pdf ??/$field.pdf ???/$field.pdf ????/$field.pdf
done
