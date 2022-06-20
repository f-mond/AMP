import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

timings_64 = pd.read_csv('output64.txt')
timings_32 = pd.read_csv('output32.txt')
timings_16 = pd.read_csv('output16.txt')
timings_8 = pd.read_csv('output8.txt')
timings_4 = pd.read_csv('output4.txt')
print(timings_64.columns)
timings_64['average'].plot(label='64')
timings_32['average'].plot(label='32')
timings_16['average'].plot(label='16')
timings_8['average'].plot(label='8')
timings_4['average'].plot(label='4')
plt.xlabel('core number')
plt.ylabel('runtime [nanoseconds]')
plt.yscale('log')
plt.legend()
plt.title('average runtime for 2 CASN operations, N=10')
plt.show()

timings_64['throughput'].plot(label='64')
timings_32['throughput'].plot(label='32')
timings_16['throughput'].plot(label='16')
timings_8['throughput'].plot(label='8')
timings_4['throughput'].plot(label='4')
plt.xlabel('core number')
plt.ylabel('throughput')
plt.yscale('log')
plt.legend()
plt.title('number of CASN operations finished in 3 seconds, N=10')
plt.show()


