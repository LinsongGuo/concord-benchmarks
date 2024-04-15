import pandas as pd

concord_file = 'overhead_results-concord.txt'
uintr_file = 'overhead_results-uintr.txt'

quantum = [100, 50, 20, 15, 10, 5,2]
N = 27

data = {}

def read_benchmarks():
    programs = []
    benchmarks = []
    with open(concord_file, 'r') as file:
        for line in file:
            words = line.split(',')
            benchmarks.append(words[0])
            programs.append(words[1])
            if len(programs) >= N:
                break
    data['benchmark'] = benchmarks
    data['program'] = programs

def read_overhead(mode, mode_file):
    with open(mode_file, 'r') as file:
        current = 0
        overhead = []
        for line in file:
            words = line.split(',')
            o = 'error' if 'error' in words[2] else '{:.2f}%'.format((float(words[2]) - 1)*100)
            overhead.append(o)
            if len(overhead) >= N:
                data['{} {}'.format(mode, quantum[current])] = overhead
                current += 1
                overhead = []
                print(N, line)

def get_order():
    order = ['benchmark', 'program']
    for q in quantum:
        order.append('concord {}'.format(q))
        order.append('uintr {}'.format(q))    
    return order

read_benchmarks()
read_overhead('concord', concord_file)
print('=================')
read_overhead('uintr', uintr_file)

# lengths = set(len(value) for value in data.values())
# if len(lengths) > 1:
#     raise ValueError("Lengths of values in 'data' dictionary are not consistent")

for d in data:
    print(d, len(data[d]), data[d])

# print(data)

order = get_order()
print('order:', order)
df = pd.DataFrame(data)#, order)
df.to_csv('all.csv', index=False)
