#!/usr/bin/python

from pylab import plotfile, show, gca

#np.savetext("data.txt", np.column_stack((x,y)), delimiter=',')


#fname = cbook.get_sample_data('data.txt', asfileobj=False)

plotfile("data.txt", (0,1), names=['$time$','$error$'])

show()
