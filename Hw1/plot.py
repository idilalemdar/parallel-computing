#!/usr/bin/env python3

import matplotlib.pyplot as plt

processors = [1,2,4,8,16,32]

def avg(lst):
    l = len(lst)
    sum = 0
    for i in range(l):
        sum += lst[i]
    return sum / l

def parse(n):
    with open(str(n) + "_out.txt") as f:
        data = f.read()
    filtered = list(filter((lambda x: x.startswith("Result")), data.strip().split("\n")))
    times = list(map(lambda x: x[38:], filtered))
    timesInt = list(map(float, times))
    return avg(timesInt)

def prepare():
    avgs = []
    for p in processors:
        avgs.append(parse(p))
    return avgs

def plot1():
    avgs = prepare()
    plt.scatter(processors, avgs)
    plt.title('Time vs. Number of processors')
    plt.xlabel('Number of processors')
    plt.ylabel('Time (s)')
    plt.savefig("plot1.png")


def plot2():
    avgs = prepare()
    single_processor = avgs[0]
    y = list(map((lambda x: (single_processor/x)), avgs))
    print(avgs)
    print(y)
    print(processors)
    plt.scatter(processors, y)
    plt.title('Speed Improvement vs. Number of processors')
    plt.xlabel('Number of processors')
    plt.ylabel('Speed improvement (times)')
    plt.savefig("plot2.png")

if __name__ == '__main__':
    plot1()
    plot2()