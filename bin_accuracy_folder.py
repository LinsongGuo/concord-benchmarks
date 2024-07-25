import os
import sys
import numpy as np
import struct
import matplotlib.pyplot as plt

path = sys.argv[1]
bench = sys.argv[2]
# subloop = [3, 5, 7, 9, 10, 11, 16, 17, 19, 20] # /data/preempt/concord/benchmarks/overhead/splash2/codes/accuracy-lu-c lu-c
# subloop = [1, 2, 3, 4, 20, 200] # /data/preempt/concord/benchmarks/overhead/phoenix/phoenix-2.0/accuracy-matrix_multiply
subloop = [0, 1, 2, 5, 10, 20, 200] # /data/preempt/concord/benchmarks/overhead/phoenix/phoenix-2.0/accuracy-pca

def read_bin_latency(binary_file_path):
  latency = []
  with open(binary_file_path, 'rb') as latency_file:
    while True:
      data = latency_file.read(8)
      if not data:
          break
      value = struct.unpack('<Q', data)[0]
      latency.append(value)

  return np.array(latency)

def calculate_time_diff(arr):
  a = []
  for i in range(len(arr)-1):
    a.append(arr[i+1]-arr[i])
  return np.array(a)

fig, axs = plt.subplots(1, len(subloop), figsize=(18, 4))
for i in range(len(subloop)):
  sub = subloop[i]

  filepath = os.path.join(path, '{}'.format(sub))
  data = read_bin_latency(os.path.join(filepath))
  data = data[data != 0]
  # print("data len:", len(data))
  
  data = calculate_time_diff(data)
  data = data/1000/2

  sorted_data = np.sort(data)[::-1]
  # print('last 30:', sorted_data[0:30])

  # get %50 %90 %99
  p50 = np.percentile(data, 50)
  p99 = np.percentile(data, 99)
  p999 = np.percentile(data, 99.9)
  avg = np.mean(data)
  mx = np.max(data)
  std = np.std(data)

  data = data[(data >= 0) & (data <= 10)]
  
  axs[i].hist(data, bins=10, edgecolor='black')
  axs[i].set_title(f'subloop = {sub}')
  axs[i].set_xlabel('Quantum')  
  axs[i].set_ylabel('Count')  

  print(f'subloop = {sub}: avg {avg:.2f} us, median {p50:.2f} us, p99 {p99:.2f} us, p99.9 {p999:.2f} us, std {std:.2f} us, max {mx:.2f} us')

plt.tight_layout()

plt.savefig('accuracy_result/{}.pdf'.format(bench))

