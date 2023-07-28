import subprocess
import os
import csv
import re

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
CI_RESULTS = os.path.join(SCRIPT_DIR, "ci_published.csv")
CONCORD_RESULTS = os.path.join(SCRIPT_DIR, "overhead_results.txt")

ci_results = {}
concord_results = {}

unroll_configs = {}
overhead_results = "overhead_results.txt"
benchs = [
    {
        "name": "splash2",
        "path": "splash2/codes/",
        "benchs": ["water-nsquared", "water-spatial", "fmm", "raytrace", "radix", "fft", "lu-c", "lu-nc", "ocean-cp", "ocean-ncp", "volrend"]
    },
    {
        "name": "parsec",
        "path": "parsec-benchmark/pkgs/",
        "benchs": ["blackscholes", "fluidanimate", "swaptions", "canneal", "streamcluster", "dedup"]
    },
    {
        "name": "phoenix",
        "path": "phoenix/phoenix-2.0/",
        "benchs": ["histogram", "kmeans", "pca", "string_match", "linear_regression", "word_count"]
    }
]

def load_ci_results():
    global ci_results
    with open(CI_RESULTS, "r") as f:
        reader = csv.reader(f)
        for row in reader:
            if row[0] == "bench":
                continue

            ci_results[row[0]] = float(row[1]) / 100   

def load_concord_results():
    global concord_results
    with open(CONCORD_RESULTS, "r") as f:
        reader = csv.reader(f)
        for row in reader:
            concord_results[row[1]] = float(row[2])  

def get_accuracy(bench_category, bench_name):

    cmd = f"python3 {os.path.join(SCRIPT_DIR, 'bin_accuracy.py')} {os.path.join(SCRIPT_DIR, 'results', 'accuracy-' + bench_name + '.bin')}"
    output = subprocess.check_output(cmd, shell=True).decode("utf-8")

    return output


def generate_overhead_table():
    # Program name, bench category, cI_RESULTS, Concord
    global ci_results
    global concord_results

    print("Program name, Bench Family, CI_RESULTS, Concord, Accuracy")

    for bench in benchs:
        for bench_name in bench["benchs"]:
            try:
                acc = get_accuracy(bench, bench_name)

                print(f"{bench_name},{bench['name']},{float(ci_results[bench_name])},{concord_results[bench_name]}, {acc}", end="")
            except:
                pass

    geo_mean_concord = 0
    geo_mean_ci = 0

    for bench in benchs:
        for bench_name in bench["benchs"]:
            try:
                geo_mean_concord += float(concord_results[bench_name])
                geo_mean_ci += float(ci_results[bench_name])
            except:
                pass
    
    geo_mean_concord = geo_mean_concord / len(concord_results)
    geo_mena_ci = geo_mean_ci / len(ci_results)

    print(f"Geo Mean, ,{geo_mena_ci},{geo_mean_concord}")
    print(f"Maximum, ,{max(ci_results.values())},{max(concord_results.values())}")


if __name__ == "__main__":
    load_concord_results()
    load_ci_results()
    generate_overhead_table()