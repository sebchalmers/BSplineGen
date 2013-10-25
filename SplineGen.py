# -*- coding: utf-8 -*-
"""
Created on Fri Nov 16 20:18:08 2012

@author: sebastien

Create a Cp/Ct table interpolation with sensitivity generation

"""


import numpy as np
from scipy import interpolate
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

import scipy.io

#Some functions
def MakeGrid(argx,argy):
    gridx   = [[argx[i] for j in range(len(argy))] for i in range(len(argx))]
    gridy   = [[argy[i] for j in range(len(argx))] for i in range(len(argy))]
    
    return np.array(gridx).T, np.array(gridy)

#### Compute data for derivatives
def Dx(P,knots_x,m,n,p):
    Px = np.zeros([m-1,n])
    for k1 in range(m-1):
        for k2 in range(n):
            Px[k1,k2] = p*(P[k1+1,k2] - P[k1,k2])/(knots_x[k1+p+1]- knots_x[k1+1])
    Px = Px.reshape(n*(m-1),)
    
    Ux = [[0. for k in range(p)]]
    Ux.append([knots_x[k] for k in range(p+1,m+1)])
    Ux.append([1.0 for k in range(p)])
    Ux = np.concatenate(Ux)

    return Px, Ux

def Dy(P,knots_y,m,n,q):
    Py = np.zeros([m,n-1])
    for k1 in range(m):
        for k2 in range(n-1):
            Py[k1,k2] = q*(P[k1,k2+1] - P[k1,k2])/(knots_y[k2+q+1]- knots_y[k2+1]) 
    Py = Py.reshape(m*(n-1),)

    Uy = [[0. for k in range(q)]]
    Uy.append([knots_y[k] for k in range(q+1,n+1)])
    Uy.append([1.0 for k in range(q)])
    Uy = np.concatenate(Uy)

    return Py, Uy

def writeData(varname, data, vartype, fileobj):
    # Function to write a given variable
    if not(isinstance(data,list)):
        Line = 'const ' + vartype+' ' + varname + ' = ' + str(data) + ';'
    else:
        
        Line = 'const ' + vartype+' ' + varname + '[' + str(len(data)) + '] = {'
        lendata = len(data)
        for k in range(lendata):
           Line += str(data[k])
           if (k < lendata-1):
              Line += ','
   
        Line += '};'

    fileobj.write(Line+'\n')


def SplineGen(beta,lambda_,C,name):

    beta_shift   = np.min(beta)
    lambda_shift = np.min(lambda_)
    
    beta -= beta_shift
    lambda_ -= lambda_shift

    
    
    betagrid, lambdagrid = MakeGrid(beta,lambda_)
    
    tck = interpolate.bisplrep(betagrid,lambdagrid,C,s=1e-2)
    
    #spline order
    knots_x     = tck[0]
    knots_y     = tck[1]
    checkpoints = tck[2]
    
    p           = tck[3]
    q           = tck[4]
    
    assert(p==3)
    assert(q==3)
    
    #P is size mxn, row major representation
    m           = len(knots_x)-p-1
    n           = len(knots_y)-q-1
    P           = tck[2].reshape(m,n)
    
    Pline       = tck[2]
    
    
    
    Px, Ux = Dx(P,knots_x,m,n,p)
    Py, Uy = Dy(P,knots_y,m,n,q)
    
    Pxx, Uxx = Dx(Px.reshape(m-1,n),      Ux, m-1 ,n   , p-1)
    Pyy, Uyy = Dy(Py.reshape(m,n-1),      Uy, m   ,n-1 , q-1)
    Pxy, _   = Dy(Px.reshape(m-1,n), knots_y, m-1 ,n   ,   q)
    
    #### Write SplineData.h ####
    
        
        
    fileobj = open(name+'.h','w')
    
    ## write variables ##
    varDictionary = {'n':              [n,              'int'],
                     'm':              [m,              'int'],
                     'p':              [p,              'int'],
                     'q':              [q,              'int'],
                     'x_shift':        [beta_shift,   'float'],
                     'y_shift':        [lambda_shift, 'float'], 
                     'knots_x':        [list(knots_x),'float'],
                     'length_knots_x': [len(knots_x),   'int'],
                     'knots_y':        [list(knots_y),'float'],
                     'length_knots_y': [len(knots_y),   'int'],
                     'P':              [list(Pline),  'float'],
                     'length_P':       [len(Pline),     'int'],
                     'Px':             [list(Px),           'float'],
                     'length_Px':      [len(Px),        'int'],
                     'Py':             [list(Py),           'float'],
                     'length_Py':      [len(Py),        'int'],
                     'Ux':             [list(Ux),           'float'],
                     'length_Ux':      [len(Ux),        'int'],
                     'Uy':             [list(Uy),           'float'],
                     'length_Uy':      [len(Uy),        'int'],
                     'Uxx':            [list(Uxx),          'float'],
                     'length_Uxx':     [len(Uxx),       'int'],
                     'Uyy':            [list(Uyy),          'float'],
                     'length_Uyy':     [len(Uyy),       'int'],
                     'Pxx':            [list(Pxx),          'float'],
                     'length_Pxx':     [len(Pxx),       'int'],
                     'Pyy':            [list(Pyy),          'float'],
                     'length_Pyy':     [len(Pyy),       'int'],
                     'Pxy':            [list(Pxy),          'float'],
                     'length_Pxy':     [len(Pxy),       'int']}
    
    for key in varDictionary.keys():
        writeData(key, varDictionary[key][0], varDictionary[key][1], fileobj)
    
    fileobj.close()
    
    return tck, betagrid, lambdagrid
    

