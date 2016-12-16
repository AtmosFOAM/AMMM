#!/usr/bin/python

import sys
import numpy as np
import matplotlib.pyplot as plt
import os.path
from scipy.interpolate import griddata
import glob


methods=(['BL2D'])
monitors=(['bell','ring'])





impath='/home/pbrowne/latex/ma_iterative/pab/'
form='.pdf'


NaN = np.nan
cmap = plt.cm.YlGn
cmap.set_bad(color = 'k', alpha = 1.)

# plots of N vs iterations
for monitor in monitors:
    print monitor
    plt.cla()
    for method in methods:
        base='results/'+monitor+'/'+method+'/'

        allN = sorted([np.int(os.path.basename(x)) for x in glob.glob(base+'*')])
        N_n = np.size(allN)
        allGamma = sorted(set([np.float(os.path.basename(x)) for x in glob.glob(base+'*/*')]))
        Gamma_n = np.size(allGamma)
        print allN
        print allGamma
        print N_n
        print Gamma_n

        iterations = np.nan*np.zeros(N_n)
        for dird in sorted(glob.glob(base+'*')):
        
            N=np.int(dird.split('/')[-1])
        
            for gammad in sorted(glob.glob(dird+'/*')):
            
            
                gamma=np.float(gammad.split('/')[-1])
 
                
                for line in open(gammad+'/'+str(method)+'Dict'):
                    if "conv" in line:
                        etol=np.float(line.split(' ')[1].split(';')[0])

                print gammad+'/equi'
                ehist=np.loadtxt(gammad+'/equi')
                itr=np.size(ehist)
                if itr > 1:
                    e=ehist[-1]
                else:
                    e=ehist
                if e > etol:
                    itr=NaN
                if itr > 1000:
                    itr = NaN
                print N,gamma
                iterations[allN.index(N)] = itr
                print N,gamma,e,itr


        plt.plot(np.array(allN),iterations,'b-',label='AFP')
        plt.xlabel('Mesh size')
        plt.ylabel('Iterations')
        plt.title(str(monitor)+' test case')
        figfile='results/N_vs_iterations_'+method+'_'+monitor+'.png'
        plt.savefig(figfile)
#        plt.show()



# plots of equi vs iterations
for monitor in monitors:
    print monitor
    plt.cla()
    for method in methods:
        base='results/'+monitor+'/'+method+'/'

        allN = sorted([np.int(os.path.basename(x)) for x in glob.glob(base+'*')])
        N_n = np.size(allN)
        allGamma = sorted(set([np.float(os.path.basename(x)) for x in glob.glob(base+'*/*')]))
        Gamma_n = np.size(allGamma)
        print allN
        print allGamma
        print N_n
        print Gamma_n

        iterations = np.nan*np.zeros(N_n)
        for dird in sorted(glob.glob(base+'*')):
        
            N=np.int(dird.split('/')[-1])
        
            for gammad in sorted(glob.glob(dird+'/*')):
            
            
                gamma=np.float(gammad.split('/')[-1])
 
                
                for line in open(gammad+'/'+str(method)+'Dict'):
                    if "conv" in line:
                        etol=np.float(line.split(' ')[1].split(';')[0])

                print gammad+'/equi'
                ehist=np.loadtxt(gammad+'/equi')
                itr=np.size(ehist)
                if itr > 1:
                    e=ehist[-1]
                else:
                    e=ehist
                if e > etol:
                    itr=NaN
                if itr > 1000:
                    itr = NaN

                if itr is not NaN:
                    plt.plot(ehist,label=str(N))
                print N,gamma
                iterations[allN.index(N)] = itr
                print N,gamma,e,itr


        plt.yscale('log')
        plt.xlabel('Iteration')
        plt.ylabel('Equidistribution')
        plt.title(str(monitor)+' test case')
        plt.legend(loc='best')
        figfile='results/equi_vs_iterations_'+method+'_'+monitor+'.png'
        plt.savefig(figfile)
#        plt.show()







'''
                # ns[cnt] = N
                # gammas[cnt] = gamma
                # iters[cnt] = itr
                # cnt+=1
                

        # plt.tricontour(ns,gammas,iters)
        # plt.colorbar()
        # plt.show()

        #Ns,Gammas,Iters=grid(ns,gammas,iters)
        X,Y = np.meshgrid(allGamma,allN)
        ax = plt.gca()
        plt.gca().patch.set_color('.25')
        #plt.imshow(iterations,cmap=cmap, extent=[np.min(allGamma), np.max(allGamma), np.min(allN), np.max(allN)], interpolation='none')
        plt.contourf(X, Y, iterations,100,cmap=cmap)
        #ax.set_aspect('auto')
        plt.xlabel('$\gamma$')
        plt.ylabel('N')
        plt.colorbar()
        pltname=impath+'N_vs_gamma_'+monitor+'_'+method+form
        plt.savefig(pltname,bbox_inches='tight')
        plt.show(block=False)
        plt.figure()



#AL with N
# contour plots of N vs gamma
for monitor in monitors:
    print monitor
    if not N_v_gamma:
        break
    for method in ['AL2D']:
        base='results/'+monitor+'/'+method+'/'

        allN = sorted([np.int(os.path.basename(x)) for x in glob.glob(base+'*')])
        N_n = np.size(allN)
        print allN
        print N_n



        iterations = np.nan*np.zeros(N_n)
        
        # numGamma = np.size(glob.glob(base+'*/*'))
        # ns = np.zeros(numGamma)
        # gammas = np.zeros(numGamma)
        # iters = np.zeros(numGamma)
        # cnt = 0

        for dird in sorted(glob.glob(base+'*')):
        
            N=np.int(dird.split('/')[-1])
        
            gammad = dird+'/1.0'
            print  gammad

            for line in open(gammad+'/'+str(method)+'Dict'):
                if "conv" in line:
                    etol=np.float(line.split(' ')[1].split(';')[0])

            print gammad+'/equi'
            ehist=np.loadtxt(gammad+'/equi')
            itr=np.size(ehist)
            if itr > 1:
                e=ehist[-1]
            else:
                e=ehist
            if e > etol:
                itr=NaN
            if itr > 1000:
                itr = NaN
            iterations[allN.index(N)] = itr
            # ns[cnt] = N
            # gammas[cnt] = gamma
            # iters[cnt] = itr
            # cnt+=1
                

        # plt.tricontour(ns,gammas,iters)
        # plt.colorbar()
        # plt.show()

        #Ns,Gammas,Iters=grid(ns,gammas,iters)
        # X,Y = np.meshgrid(allGamma,allN)
        # ax = plt.gca()
        # plt.gca().patch.set_color('.25')
        #plt.imshow(iterations,cmap=cmap, extent=[np.min(allGamma), np.max(allGamma), np.min(allN), np.max(allN)], interpolation='none')
        plt.plot(allN, iterations,linestyle='-',marker='o')
        #ax.set_aspect('auto')
        plt.xlabel('N')
        plt.ylabel('Iterations')
        #plt.colorbar()
        pltname=impath+'N_vs_iterations_'+monitor+'_'+method+form
        plt.savefig(pltname,bbox_inches='tight')
        plt.show(block=False)
        plt.figure()


#PMA gamma vs dt contours:
for monitor in monitors:
    if not PMA_gamma_v_dt:
        break
    method='PMA2D'
    base='results/'+monitor+'/'+method+'/'

    allN = sorted([np.int(os.path.basename(x)) for x in glob.glob(base+'*')])
    N_n = np.size(allN)
    allGamma = sorted(set([np.float(os.path.basename(x)) for x in glob.glob(base+'*/*')]))
    Gamma_n = np.size(allGamma)
    allDt = sorted(set([np.float(os.path.basename(x)) for x in glob.glob(base+'*/*/*')]))
    Dt_n = np.size(allDt)
    #print allN
    #print allGamma
    #print N_n
    #print Gamma_n
    #print allDt
    #print Dt_n



    iterations = np.nan*np.zeros([Dt_n,Gamma_n])
        
    # numGamma = np.size(glob.glob(base+'*/*'))
    # ns = np.zeros(numGamma)
    # gammas = np.zeros(numGamma)
    # iters = np.zeros(numGamma)
    # cnt = 0
    
    for dird in sorted(glob.glob(base+'*')):
        
        N=np.int(dird.split('/')[-1])
        #print 'dird = ',dird,'N = ',N
        for gammad in sorted(glob.glob(dird+'/*')):
            

            gamma=np.float(gammad.split('/')[-1])
            #print 'gammad = ',gammad,'gamma = ',gamma
            for dtd in sorted(glob.glob(gammad+'/*')):
                dt = np.float(dtd.split('/')[-1])
                #print 'dtd = ',dtd,' dt = ',dt
                for line in open(dtd+'/'+str(method)+'Dict'):
                    if "conv" in line:
                        etol=np.float(line.split(' ')[1].split(';')[0])

                #print dtd+'/equi'
                ehist=np.loadtxt(dtd+'/equi')
                itr=np.size(ehist)
                if itr > 1:
                    e=ehist[-1]
                else:
                    e=ehist
                if e > etol:
                    itr=NaN
                #print N,gamma,dt
                iterations[allDt.index(dt),allGamma.index(gamma)] = itr
                print N,gamma,dt,e,itr
                # ns[cnt] = N
                # gammas[cnt] = gamma
                # iters[cnt] = itr
                # cnt+=1
                

    # plt.tricontour(ns,gammas,iters)
    # plt.colorbar()
    # plt.show()

    #Ns,Gammas,Iters=grid(ns,gammas,iters)
    X,Y = np.meshgrid(allGamma,allDt)
    ax = plt.gca()
    plt.gca().patch.set_color('.25')
    #plt.imshow(iterations,cmap=cmap, extent=[np.min(allGamma), np.max(allGamma), np.min(allN), np.max(allN)], interpolation='none')
    plt.contourf(X, Y, iterations,100,cmap=cmap)
    #ax.set_aspect('auto')
    plt.xlabel('$\gamma$')
    plt.ylabel('$\delta t$')
    plt.colorbar()
    pltname=impath+'N_'+str(N)+'_dt_vs_gamma_'+monitor+'_'+method+form
    plt.savefig(pltname,bbox_inches='tight')
    plt.show()

    # plt.figure()
    # plt.imshow(iterations,interpolation=None,cmap=cmap)
    # plt.colorbar()
    # plt.show()
'''
