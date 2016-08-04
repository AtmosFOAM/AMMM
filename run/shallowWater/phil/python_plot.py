#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import sys
import glob
import time



water=plt.cm.BrBG(0.7)
orogr=plt.cm.BrBG(0.15)
umap = plt.cm.PRGn

plt.ion()
fig = plt.figure(figsize=(15,6))
ax = plt.gca()
#fig.patch.set_facecolor('black')
#ax.set_axis_bgcolor('black')

#ax.set_edgecolor('white')
#ax.set_labelcolor('white')
#ax.set_aspect(400)

htmax = 0.0
xmin = 99999999999999.9
xmax = 0.0
umin = 99999999999999.9
umax = 0.0

for f in sorted(glob.glob('[0-9]*') , key=lambda name: int(float(name))):
    print f
    data = np.loadtxt(f+'/data')
    ht = data[:,2]
    htmax = max(htmax,max(ht))
    x = data[:,0]
    xmax = max(np.max(x),xmax)
    xmin = min(np.min(x),xmin)
    u = data[:,3]
    umax = max(np.max(u),umax)
    umin = min(np.min(u),umin)

n = np.size(u)
print htmax,umin,umax,xmin,xmax
ulim = max(abs(umin),abs(umax))


for f in sorted(glob.glob('[0-9]*') , key=lambda name: int(float(name))):
    print f
    data = np.loadtxt(f+'/data')
    hTot = np.loadtxt(f+'/inth')
    x = data[:,0]
    h = data[:,1]
    ht = data[:,2]
    u = data[:,3]
    h0 = ht - h
    plt.clf()
    #plt.ylim([0,htmax])
    plt.imshow(np.reshape(u,(1,n)),cmap=umap,aspect='auto',extent=(xmin,xmax,0,1.2*htmax),vmin=-ulim,vmax=ulim)
    plt.fill_between(x,0,ht,facecolor=water,color=water)
    #the following line is shorthand for making the orography appear for each cell
    #not from cell centre to cell centre as would happen with:
    #plt.fill_between(x,0,h0,facecolor=orogr,color=orogr)
    plt.fill_between([j for i in zip(x-x[0],x+x[0]) for j in i],0,[j for i in zip(h0,h0) for j in i],facecolor=orogr,color=orogr)
    plt.colorbar(orientation='horizontal')
    plt.title('Time = '+str(int(float(f))/3600)+'hrs. int(h) = '+str(hTot))
    if ( f == '0'):
        plt.pause(1)
    else:
        plt.pause(0.01)
    
#plt.plot(x,h,animated=True)

plt.show()
