#!/usr/bin/env python
# -*- coding: utf-8 -*-

from math import *
import struct
import traceback

import matplotlib.pyplot as plt

import numpy as np


class ErrSeries(object):
    def __init__(self, name):
        self.name = name

    @staticmethod
    def get_from_file(name):
        result = ErrSeries(name)
        
        file = open(".accuracy/"+name+".f32","rb")
        result.low,  = struct.unpack("f", file.read(4))
        result.high, = struct.unpack("f", file.read(4))
        data = file.read()
        file.close()

        result.data = np.array( struct.unpack("f"*(len(data)//4), data) )
        result.xs = np.linspace(result.low,result.high,len(result.data))

        return result
    @staticmethod
    def get_from_const(name, low,high, value):
        result = ErrSeries(name)

        result.low  = low
        result.high = high

        result.xs = np.array([low,high])
        result.data = np.array([value]*2)

        return result

def create_plot(fnname,series_list):
    series0 = series_list[0]
    for series in series_list:
        assert series.low  == series0.low
        assert series.high == series0.high

    plots = []
    colors = [
        "#1F77B4", "#FF7F0E", "#2CA02C", "#D62728",
        "#9467BD", "#8C564B", "#E377C2", "#7F7F7F",
        "#BCBD22", "#17BECF"
    ]
    for series in series_list:
        i = len(plots)
        if len(series.xs) == 2:
            plot, = plt.plot(series.xs,series.data, label=series.name, linewidth=2,c=colors[i],zorder=i)
        else:
            plot  = plt.scatter(series.xs,series.data, label=series.name, s=2,c=colors[i],zorder=i)
##        plot, = plt.plot(series.xs,series.data, label=series.name, linewidth=1)
        plots.append(plot)

    plt.legend(handles=plots,loc="upper center")

    #plt.xticks(list(plt.xticks()[0])+[ pi, 2.0*pi ])
##    plt.xticks(
##        [0.0, pi*0.5, pi, pi*1.5, 2.0*pi],
##        ["0","π/2","π","3π/2","2π"]
##    )
    plt.xticks(
        [-2.0*pi, -pi, 0, pi, 2.0*pi, 3.0*pi, 4.0*pi],
        ["-2π","-π","0","π","2π","3π","4π"]
    )

    #plt.yscale("log")

    plt.xlabel("x")
    plt.ylabel("|error|")
    plt.title("Absolute Error (%s)"%(fnname))

    plt.grid(True)

    max_val = max([ np.max(series.data) for series in series_list ])
    plt.ylim(( 0.0, max_val ))

    plt.show()


def main():
    sin_32f_minitrig = ErrSeries.get_from_file("sin-32f-minitrig")
    sin_32f_msvc = ErrSeries.get_from_file("sin-32f-msvc")
    eps = ErrSeries.get_from_const("machine epsilon", sin_32f_minitrig.low,sin_32f_minitrig.high, 2.0**-23.0)
    create_plot("sin",[
        eps,
        sin_32f_minitrig,
        sin_32f_msvc
    ])

    #plt.savefig("test.png")
    
if __name__ == "__main__":
    try:
        main()
    except:
        traceback.print_exc()
        input()
