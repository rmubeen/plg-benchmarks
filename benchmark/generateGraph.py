#!/bin/python3

import sys
import numpy as np
import matplotlib.pyplot as plt

def draw_graph(titles, data):
    x = list(range(len(data[0])))
    print("Data points:", len(data[0]), ". Titles:", titles)

    for i in data:
        plt.plot(x, i)

    plt.legend(titles)
    plt.grid(True)
    plt.xlabel('Time')
    plt.ylabel('Memory')

    plt.show()

def read_file(fname, skip_rows_top, skip_rows_bottom, columns):
    fin = open(fname, "r")
    content = fin.readlines()
    titles = content[1].split()
    if(len(columns) == 0):
        columns = list(range(len(titles)))
    print("Columns:", columns)

    titles = [titles[i] for i in columns]
    data = [[] for i in titles]
    if(skip_rows_top == 0):
        skip_rows_top = 2

    for i in range(skip_rows_top, len(content)-skip_rows_bottom):
        cur_data = content[i].split()
        cur_data = [float(i) for i in cur_data]

        for j in range(len(columns)):
            data[j].append(cur_data[columns[j]])

    return titles, data

if(__name__ == "__main__"):
    arg_req = ["<filename>", "<top rows to skip>", "<bottom rows to skip>"]

    arg_len = len(sys.argv) - 1
    if (arg_len < len(arg_req)):
        print("ERR: got", arg_len, "arguments, requires", len(arg_req), ":", " ".join(sys.argv))
        print("Usage:\n\tpython(3)", sys.argv[0], " ".join(arg_req), "<columns to draw graph>")
        exit()

    titles, data = read_file(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), [int(i) for i in sys.argv[4:]])
    draw_graph(titles, data)