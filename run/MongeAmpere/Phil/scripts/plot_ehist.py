#!/usr/bin/env python
import numpy as np
import timeit
import os
import glob
from sys import exit
from subprocess import call
import matplotlib.pyplot as plt

plt.cla()
cmap=plt.cm.Set1
if cmap == plt.cm.Dark2:
    cint = 1.0/8.0
elif cmap == plt.cm.Set1:
    cint = 1.0/8.0
    
cnt=0

monitors=['ring','bell']
methods=['PMA','BL']
Ns=['60']

iters=[]
times = []
form='.pdf'
gamma = []
dt = []
impath='/home/pbrowne/latex/ma_iterative/shit/'

for monitor in monitors:
    for N in Ns:
        for method in methods:
            if method == 'FP':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'*/'
            elif method == 'AL':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'1.0/'
            elif method == 'BL':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'1.0/'
            elif method == 'PMA':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'*/*/'
            
            cnt = 0.
            for folder in sorted(glob.glob(path)):
                print folder
                if method == 'FP':
                    gamma=folder.split('/')[4]
                elif method == 'AL':
                    gamma=folder.split('/')[4]
                elif method == 'BL':
                    gamma=folder.split('/')[4]
                elif method == 'PMA':
                    gamma=folder.split('/')[4]
                    dt=folder.split('/')[5]
                equifile=folder+'equi'

                convfile=folder+method+'2DDict'
                for line in open(convfile):
                    if 'conv' in line:
                        conv=np.float(line.split(' ')[1].split(';')[0])

                e = np.loadtxt(equifile)
                iters.append(np.size(e)-1)

                if method == 'PMA':
                    label=method+' ($\gamma='+str(gamma)+', \delta t = '+str(dt)+'$)'
                    marker='o'
                elif method == 'FP':
                    label=method+' ($\gamma='+str(gamma)+'$)'
                    marker='.'
                elif method == 'AL':
                    label=method+' ($\gamma='+str(gamma)+'$)'
                    label=method
                    marker=''
                elif method == 'BL':
                    label='AFP'+' ($\gamma='+str(gamma)+'$)'
                    label='AFP'
                    marker=''



                if np.size(e) > 1 and np.size(e) < 1000:
                    if e[-1] < conv:
                        print gamma,np.size(e)-1,e[-1]
                        #labels.append(label)
                        color=cmap(cnt)
                        if method == 'PMA':
                            color=cmap( ((np.float(dt)/0.05)-2)%8*cint)
                            marker=''
                            linestyle='-'
                            x = np.arange(np.size(e))
                            if gamma == '0.45':
                                marker=''
                                linestyle='--'
                            elif gamma == '0.5':
                                marker=''
                                linestyle='-.'
                            elif gamma == '0.55':
                                marker=''
                                linestyle=':'
                            elif gamma == '0.6':
                                marker=''
                                linestyle='-'
                            elif gamma == '0.65':
                                marker='.'
                                linestyle=''
                                e = np.append(e[0:-1:15],e[-1])
                                x = np.append(x[0:-1:15],x[-1])
                            elif gamma == '0.7':
                                marker='*'
                                linestyle=''
                                e = np.append(e[5:-1:15],e[-1])
                                x = np.append(x[5:-1:15],x[-1])
                            elif gamma == '0.75':
                                marker='o'
                                linestyle=''
                                e = np.append(e[10:-1:15],e[-1])
                                x = np.append(x[10:-1:15],x[-1])
                            elif gamma == '0.8':
                                marker='^'
                                linestyle=''
                                e = np.append(e[3:-1:15],e[-1])
                                x = np.append(x[3:-1:15],x[-1])
                        elif method == 'FP':
                            x = np.arange(np.size(e))
                            if np.float(gamma) >= 3.0:
                                marker=''
                                linestyle=':'
                            else:
                                marker=''
                                linestyle='-'
                        elif method == 'AL':
                            x = np.arange(np.size(e))
                            color=cmap( ((np.float(gamma)-0.8)/0.05)*cint)
                            color='k'
                            marker = ''
                            linestyle='-'
                        elif method == 'BL':
                            x = np.arange(np.size(e))
                            color=cmap( ((np.float(gamma)-0.8)/0.05)*cint)
                            color='k'
                            marker = ''
                            linestyle='-'

                        else:
                            linestyle='-'
                        plt.semilogy(x,e,label=label,color=color,marker=marker,linestyle=linestyle,markeredgecolor=color,markersize=5)
                        #colors.append(cmap(cnt))
                        cnt+=cint
                        cnt=np.mod(cnt,1.0)
                    else:
                        print method,N,monitor,gamma,dt,' did not converge'
                else:
                    print  method,N,monitor,gamma,dt,' did not converge'
        plt.xlabel('Iteration number')
        plt.ylabel('Equidistribution (CV of $m|I+H|$)')    
        #plt.xlim(xmax=160)
        plt.legend(ncol=2, bbox_to_anchor=(2., 1))
        pltname=impath+'eq_vs_iter_'+monitor+repr(methods).replace(" ", "")+'_'+N+form
        plt.savefig(pltname,bbox_inches='tight')#, pad_inches = 0)
#        plt.show()


        plt.figure()

pmabell=False
pmaring=False
fpbell=False
fpring=False
albell=False
alring=False
blbell=False
blring=False
nuring=False
nubell=False


for monitor in monitors:
    for N in ['60']:
        for method in ['PMA','FP','BL','NEWTON2D-pabd','NEWTON2D-pabl','NEWTON2D-pabu']:
            
            if method == 'FP':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'*/'
            elif method == 'AL':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'1.0/'
            elif method == 'BL':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'1.0/'
            elif method == 'PMA':
                base='results/'+monitor+'/'+method+'2D/'+N+'/'
                path=base+'*/*/'
            elif method == 'NEWTON2D-pabd':
                base='results/'+monitor+'/NEWTON2D-pab/'+N+'/'
                path=base+'downwind/'
                print path
            elif method == 'NEWTON2D-pabl':
                base='results/'+monitor+'/NEWTON2D-pab/'+N+'/'
                path=base+'linear/'
                print path
            elif method == 'NEWTON2D-pabu':
                base='results/'+monitor+'/NEWTON2D-pab/'+N+'/'
                path=base+'upwind/'
                print path
            
            #cnt = 0.
            for folder in sorted(glob.glob(path)):
                print folder
                if method == 'FP':
                    gamma=folder.split('/')[4]
                elif method == 'AL':
                    gamma=folder.split('/')[4]
                elif method == 'BL':
                    gamma=folder.split('/')[4]
                elif method == 'PMA':
                    gamma=folder.split('/')[4]
                    dt=folder.split('/')[5]
                elif method == 'NEWTON2D-pab':
                    gamma=folder.split('/')[4]
                
                equifile=folder+'equi'
                timefile=folder+'time'
                convfile=folder+method+'2DDict'

                e = np.loadtxt(equifile)
                iteration = np.size(e)-1
                time=np.loadtxt(timefile)
                

                if monitor == 'ring':
                    if method == 'PMA':
                        marker='o'
                        markersize=3
                        color=cmap(0*cint)
                        if not pmaring and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='PMA Ring'
                            pmaring = True
                        else:
                            legend=''
                    elif method == 'FP':
                        marker='*'
                        markersize=5
                        color=cmap(1*cint)
                        if not fpring and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='FP Ring'
                            fpring = True
                        else:
                            legend=''                        
                    elif method == 'AL':
                        marker='^'
                        markersize=5
                        color=cmap(3*cint)
                        #color='k'
                        if not alring and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='AL Ring'
                            alring = True
                        else:
                            legend=''
                    elif method == 'BL':
                        marker='H'
                        markersize=5
                        color=cmap(2*cint)
                        color='k'
                        if not blring and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='AFP Ring'
                            blring = True
                        else:
                            legend=''
                    elif method == 'NEWTON2D-pabd' or method == 'NEWTON2D-pabl'  or method == 'NEWTON2D-pabu':
                        marker='x'
                        markersize=5
                        color=cmap(2*cint)
                        color='cyan'
                        if not nuring and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='Newton Ring'
                            nuring = True
                        else:
                            legend=''
                elif monitor == 'bell':
                    if method == 'PMA':
                        marker='o'
                        markersize=3
                        color=cmap(4*cint)
                        if not pmabell and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='PMA Bell'
                            pmabell = True
                        else:
                            legend=''
                    elif method == 'FP':
                        marker='*'
                        markersize=5
                        color=cmap(6*cint)
                        if not fpbell and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='FP Bell'
                            fpbell = True
                        else:
                            legend=''

                    elif method == 'AL':
                        marker='^'
                        markersize=5
                        color=cmap(7*cint)
                        #color='k'
                        if not albell and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='AL Bell'
                            albell = True
                        else:
                            legend=''
                    elif method == 'BL':
                        marker='H'
                        markersize=5
                        color=cmap(2*cint)
                        #color='k'
                        if not blbell and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='AFP Bell'
                            blbell = True
                        else:
                            legend=''
                    elif method == 'NEWTON2D-pabd' or method == 'NEWTON2D-pabl'  or method == 'NEWTON2D-pabu':
                        marker='x'
                        markersize=5
                        color=cmap(2*cint)
                        color='magenta'
                        if not nubell and iteration > 0 and iteration < 1000 and time > 0.:
                            legend='Newton Bell'
                            nubell = True
                        else:
                            legend=''


                if iteration > 0 and iteration < 1000 and time > 0.:
                    print 'hello',time,iteration,method
                    plt.plot(iteration,time,color=color,marker=marker,markersize=markersize,linestyle='',markeredgecolor=color,label=legend)#,color=color,marker=marker)

plt.xlabel('Iterations')
plt.ylabel('Wall-clock Time (seconds)')    
#plt.xlim(xmax=160)
plt.legend(ncol=2,loc=0)
pltname=impath+'cpus_iter_'+N+form
plt.savefig(pltname,bbox_inches='tight')#, pad_inches = 0)






exit()

