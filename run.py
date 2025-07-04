import subprocess
import os
import csv
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

unroll_configs = {}
mechanism = None
overhead_results = "overhead_results.txt"
if len(sys.argv) > 2:
    mechanism = sys.argv[1]
    overhead_results = sys.argv[2]
else:
    print("At least two arguments")
    exit(-1)

benchs = [
    {
        "name": "splash2",
        "path": "splash2/codes/",
        "benchs": ["water-nsquared", "water-spatial", "ocean-cp",  "volrend", "fmm", "raytrace", "radix", "fft", "lu-c", "lu-nc", "cholesky", "radiosity"]
    },
    {
        "name": "phoenix",
        "path": "phoenix/phoenix-2.0/",
        "benchs": ["histogram", "kmeans", "pca", "string_match", "linear_regression", "word_count", "matrix_multiply", "reverse_index"]
    },
    {
        "name": "parsec",
        "path": "parsec-benchmark/pkgs/",
        "benchs": ["blackscholes", "fluidanimate", "swaptions", "canneal", "streamcluster"]
    }
]

def load_configs():
    global unroll_configs
    with open("configs/unroll.conf", "r") as f:
        reader = csv.reader(f)
        for row in reader:
            unroll_configs[row[0]] = [row[1], row[2]]    
    unroll_configs.pop("bench")


def run_bench(bench_category, bench_name, accuracy=0, pass_type="cache"):

    os.chdir(os.path.join(SCRIPT_DIR, bench_category["path"]))

    if bench_name in ['water-nsquared', 'water-spatial', 'ocean-np', 'kmeans', 'streamcluster']:
        runs = 55
    elif bench_name in ['fluidanimate', 'dedup', 'streamcluster', 'word_count', 'reverse_index', 'kmeans', 'pca', 'fmm', 'histogram', 'string_match', 'water-nsquared', 'water-spatial', 'ocean-ncp', 'volrend', 'cholesky']:
        runs = 29
    elif bench_name == 'canneal':
        runs = 11
    else: 
        runs = 5

    # To save time, uncomment the following line to run each benchmark program only once.
    # runs = 1

    cmd = f" RUNS={1 if accuracy else runs } \
                                        NOT_CONCORD={0 if mechanism == 'concord' else 1} \
                                        MODIFIED_SUBLOOP_COUNT={int(unroll_configs[bench_name][1])} \
                                        UNROLL_COUNT={int(unroll_configs[bench_name][0])} \
                                        ACCURACY_TEST={accuracy} CONCORD_PASS_TYPE={pass_type} \
                                        ./perf_test.sh " + bench_name
    
    print("Running command: ", cmd)
    output = subprocess.check_output(cmd, shell=True).decode("utf-8")

    if accuracy:
        move_accuracy_result(bench_category["name"], bench_name)
    else:
        try:
            overhead = re.search("Overhead: ((\d+)?\.\d+)", output).group(1)
        except:
            overhead = "error"

        with open(os.path.join(SCRIPT_DIR, overhead_results), "a") as f:
            f.write(f"{bench_category['name']},{bench_name},{overhead}\n")

    return output


def move_accuracy_result(bench_category, bench_name):
    os.system(f"mv {bench_category}_stats/accuracy-{bench_name}.txt {SCRIPT_DIR}/results/accuracy-{bench_name}.bin")

def run_category(bench_category, timeliness=False, overhead=True):    
    os.chdir(os.path.join(SCRIPT_DIR, bench_category["path"]))

    if timeliness:
        for bench in bench_category["benchs"]:
            print("Running", bench)
            run_bench(bench_category, bench, accuracy=1, pass_type="cache")
    
    if overhead:
        for bench in bench_category["benchs"]:
            print("Running", bench)
            run_bench(bench_category, bench, accuracy=0, pass_type="cache")

if __name__ == "__main__":
    
    print("Running benchmarks")
    print("=================================")

    load_configs()

    run_category(benchs[0], timeliness=False, overhead=True)
    run_category(benchs[1], timeliness=False, overhead=True)
    run_category(benchs[2], timeliness=False, overhead=True)