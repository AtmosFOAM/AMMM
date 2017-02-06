# Calculate the speed at which the vortices propagate each other by integrating
# the voriticty to find the velocity at the location of the other vortex

from scipy.stats import norm
from numpy import pi
from numpy import sqrt

# Distance between each vortex centre and the midline
R = 9e3

# Gaussian vortex radius
rv = 2e3

# Vorticity maximum
q0 = 5e-2

# Speed at which one vortex propagates the other
up = q0*rv*(norm.cdf(2*R,0,rv) - norm.cdf(0,0,rv))*sqrt(2*pi)

# Domain size
L = 50e3
# Background wind speed
U = 10.

# Total propagation speed
ut = U + up

# Time for one revolution
T = L/ut

# In reality, maximum velocity is 62.8879 m/s

# Approximate time for one resolution T = 3150
# Which would mean that we need up=5.8730158730158735

