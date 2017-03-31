#!/usr/bin/env python
# -*- coding: utf-8 -*-

from math import *
import struct
import traceback

import matplotlib
#matplotlib.use("wx")
import matplotlib.pyplot as plt

import numpy as np


class SeriesBase(object):
    def __init__(self, name):
        self.name = name

    def read_from_file(self, path, datatype):
        file = open(path,"rb")
        self.low,  = struct.unpack("f", file.read(4))
        self.high, = struct.unpack("f", file.read(4))
        data = file.read()
        file.close()

        self.data = np.array( struct.unpack(datatype*(len(data)//4), data) )
        self.xs = np.linspace(self.low,self.high,len(self.data))
class ErrSeries(SeriesBase):
    def __init__(self, name):
        SeriesBase.__init__(self,name)

    @staticmethod
    def get_from_file(name):
        result = ErrSeries(name)
        result.read_from_file(".accuracy/"+name+".f32","f")
        return result
    @staticmethod
    def get_from_const(name, low,high, value):
        result = ErrSeries(name)

        result.low  = low
        result.high = high

        result.xs = np.array([low,high])
        result.data = np.array([value]*2)

        return result
class LatencySeries(SeriesBase):
    def __init__(self, name):
        SeriesBase.__init__(self,name)

    @staticmethod
    def get_from_file(name):
        result = LatencySeries(name)
        result.read_from_file(".speed/"+name+".f32","f")
        return result

#http://matplotlib.org/api/pyplot_api.html#matplotlib.pyplot.legend
#plt.yscale("log")
    
colors = [ "#1F77B4", "#FF7F0E", "#2CA02C", "#D62728", "#9467BD", "#8C564B", "#E377C2", "#7F7F7F", "#BCBD22", "#17BECF" ]
def create_plot_accuracy(fnname,series_list):
    for series in series_list: assert series.low==series_list[0].low and series.high==series_list[0].high

    plots = []
    for series in series_list:
        i = len(plots)
        if len(series.xs) == 2:
            plot, = plt.plot(series.xs,series.data, label=series.name, linewidth=2,c=colors[i],zorder=i)
        else:
            plot  = plt.scatter(series.xs,series.data, label=series.name, s=2,c=colors[i],zorder=i)
        plots.append(plot)

    plt.axvspan(0.0,2.0*pi, color="blue", alpha=0.1)

    plt.legend(loc="upper left")

    plt.xticks(
        [-2.0*pi, -pi, 0, pi, 2.0*pi, 3.0*pi, 4.0*pi],
        ["-2π","-π","0","π","2π","3π","4π"]
    )

    plt.xlabel("x (radians)")
    plt.ylabel("|Error|")
    plt.title("Absolute Error (%s)"%(fnname))

    plt.grid(True)

    max_val = max([ np.max(series.data) for series in series_list ])
    #max_val = 5e-7
    plt.ylim(( 0.0, max_val ))

def create_plot_latency(fnname,series_list):
    for series in series_list: assert series.low==series_list[0].low and series.high==series_list[0].high

    plots = []
    for series in series_list:
        i = len(plots)
##        plot, = plt.plot(series.xs,series.data, label=series.name, linewidth=2,c=colors[i],zorder=i)
        plot  = plt.scatter(series.xs,series.data, label=series.name, s=2,c=colors[i],zorder=i)
        plots.append(plot)

    plt.axvspan(0.0,2.0*pi, color="blue", alpha=0.1)

    plt.legend()#loc="upper left")

    plt.xticks(
        [-2.0*pi, -pi, 0, pi, 2.0*pi, 3.0*pi, 4.0*pi],
        ["-2π","-π","0","π","2π","3π","4π"]
    )

    plt.xlabel("x (radians)")
    plt.ylabel("Average Latency (ns)")
    plt.title("Latency (%s)"%(fnname))

##    plt.yscale("log")

    plt.grid(True)

    max_val = max([ np.max(series.data) for series in series_list ])
##    max_val = 1000
    plt.ylim(( 0.0, max_val ))


def main():
    x,y = 50,50
    w,h = 600,400
    pad = 20
    dpi = 100
    
    fig1 = plt.figure(figsize=(w//dpi,h//dpi))
    sin_32f_minitrig = ErrSeries.get_from_file("sin-32f-minitrig")
    sin_32f_msvc = ErrSeries.get_from_file("sin-32f-msvc")
    eps = ErrSeries.get_from_const("machine epsilon", sin_32f_minitrig.low,sin_32f_minitrig.high, 2.0**-23.0)
    create_plot_accuracy("sin",[
        eps,
        sin_32f_minitrig,
        sin_32f_msvc
    ])
    plt.get_current_fig_manager().window.wm_geometry("+%d+%d"%(x,y))

    fig2 = plt.figure(figsize=(w//dpi,h//dpi))
    sin_32f_minitrig = LatencySeries.get_from_file("sin-32f-minitrig")
    sin_32f_msvc = LatencySeries.get_from_file("sin-32f-msvc")
    create_plot_latency("sin",[
        sin_32f_minitrig,
        sin_32f_msvc
    ])
    plt.get_current_fig_manager().window.wm_geometry("+%d+%d"%(x+w+pad,y))

    #plt.savefig("test.png")

    plt.show()

if __name__ == "__main__":
    try:
        main()
    except:
        traceback.print_exc()
        input()
