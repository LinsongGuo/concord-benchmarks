import pandas as pd

uintr_file = 'overhead_results-uintr.txt'
concord_file = 'overhead_results-concord.txt'
signal_file = 'overhead_results-signal.txt'

quantum = [200, 100, 50, 30, 20, 15, 10, 5, 3]
N = 25

data = {}

def read_benchmarks():
    programs = []
    benchmarks = []
    with open(uintr_file, 'r') as file:
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
            if not line.strip():
                continue
            if '=====' in line:
                continue
         
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
        order.append('uintr {}'.format(q))    
        order.append('concord {}'.format(q))
        order.append('signal {}'.format(q))
    return order

read_benchmarks()
read_overhead('uintr', uintr_file)
read_overhead('concord', concord_file)
read_overhead('signal', signal_file)

for d in data:
    print(d, len(data[d]), data[d])

order = get_order()
print('order:', order)
df = pd.DataFrame(data)
df.to_csv('all.csv', index=False)
