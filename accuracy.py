import os
import sys

# get path as argument
path = sys.argv[1]

# get all files in path with starts with "accuracy"
files = [f for f in os.listdir(path) if f.startswith("accuracy")]

# files include numbers interval = 10

for file in files:
    # read file
    with open(os.path.join(path, file), "r") as f:
        data = f.readlines()
    
    # give median interval size and 99.9 percentile
    data = [int(x) for x in data]

    # remove first %10
    data = data[int(len(data)*0.1):]
    data.sort()

    print("File:", file, end=" ")
    print(data[int(len(data)*0.5)], end=" ")
    print(data[int(len(data)*0.99)])
    # print("Median: ", data[int(len(data)/2)])
    # print("99 percentile: ", data[int(len(data)*0.9)])
    # print("=================================")
