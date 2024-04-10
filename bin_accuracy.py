import os
import sys
import numpy as np
import struct

file = sys.argv[1]

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

data = read_bin_latency(os.path.join(file))

print(data)
# throw 0s
data = data[data != 0]
print(len(data))
# data = data[int(len(data)*0.1):]
data = calculate_time_diff(data)

# get %50 %90 %99
p50 = np.percentile(data, 50)
p99 = np.percentile(data, 99)

name = file.split("accuracy-")[1].split(".bin")[0]
# print(f"{name},{p50}")
print(f"{p50}")