import os
import sys
import numpy as np
import struct
import matplotlib.pyplot as plt

path = sys.argv[1]
bench = sys.argv[2]

endiannes="little" # Please change as per your machine 
data_chunk_size=8  # sizeof(long long) on s2

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

data = read_bin_latency(os.path.join(path, bench))

# throw 0s
data = data[data != 0]
print(len(data))
# data = data[int(len(data)*0.1):]
data = calculate_time_diff(data)
data = data/1000/2

print('min: {}, max: {}'.format(np.min(data), np.max(data)))

sorted_data = np.sort(data)[::-1]
print(sorted_data[0:30])

p50 = np.percentile(data, 50)
p99 = np.percentile(data, 99)
avg = np.mean(data)
std = np.std(data)

data = data[(data >= 0) & (data <= 100)]


plt.hist(data, bins=100, edgecolor='black')
plt.xlabel('Quantum')
plt.ylabel('Count')
# plt.xlim(0, 30) 
plt.title('Histogram')

# # Save the plot to a PDF file
plt.savefig(os.path.join(path, '{}.pdf'.format(bench)))

print(f"p50: {p50}")
print(f"p99: {p99}")
print(f"avg: {avg}")
print(f"std: {std}")

