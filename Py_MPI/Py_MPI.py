from mpi4py import MPI
import random
import sys
from math import sin, cos, sqrt
import time

comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()

def monte_carlo_integral(f, a, b, n):
    s = 0
    for i in range(n):
        x = random.uniform(a, b)
        s += f(x)
    return s * (b - a) / n

if rank == 0:
    filename = "config.txt"
    with open(filename) as f:
        a, b, res = map(float, f.readline().split())
    n = int(sys.argv[1])
    integral_sum = 0
    for i in range(1, size):
        comm.send((a, b, n), dest=i)
    start = time.time()
    for i in range(size):
        if i == 0:
            integral_sum += monte_carlo_integral(lambda x: (pow(sin(x) * cos(2 * x), 2) - sqrt(6 * x)), a, b, n)
        else:
            integral_sum += comm.recv(source=i)
    tt = time.time() - start
    print("TIME: ",tt)
    print("Res: ",integral_sum / size, "test_res", res)
else:
    a, b, n = comm.recv(source=0)
    integral = monte_carlo_integral(lambda x: (pow(sin(x) * cos(2 * x), 2) - sqrt(6 * x)), a, b, n)
    comm.send(integral, dest=0)

