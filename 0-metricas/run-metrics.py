from subprocess import check_output
import sys
import re
import matplotlib.pyplot as plt
import pandas as pd

def get_metrics(t1, tp, p):
    speedup = t1 / tp
    eficiencia = speedup / p
    if p != 1:
        fraccion_serial = ((1 / speedup) - (1 / p)) / (1 - 1 / p)
    else:
        fraccion_serial = None
    return {
        "threads" : p,
        "T(1)": t1,
        "T(p)": tp,
        "speedup": speedup,
        "eficiencia": eficiencia,
        "fraccion_serial": fraccion_serial
    }

def print_dict(d):
    for key in d:
        print(key, ":", d[key])

def generate_graphics(data):
    df = pd.DataFrame(data)

    # plt.subplot(2,1,1)
    plt.plot(df["threads"], df["T(p)"] / 1000000, label="T(p) in seconds")
    # plt.legend()

    # plt.subplot(2,1,2)
    plt.plot(df["threads"], df["speedup"], label="speedup")
    plt.plot(df["threads"], df["eficiencia"], label="eficiencia")
    plt.plot(df["threads"], df["fraccion_serial"], label="fraccion serial")
    plt.title("Metrics")
    plt.legend()

    plt.show()
    return

def main():
    threads = [1,2,4,6,10,12,20,50,100]
    execution_mean = [0] * len(threads)
    iters = int(sys.argv[1]) if len(sys.argv) == 2 else 1

    for idx, num_threads in enumerate(threads):
        execution_times = [0] * iters
        for i in range(iters):
            output = check_output(["./a.out", str(num_threads)])
            re_total = re.search("(?<=ejecucion: )\d*", str(output))
            execution_times[i] = int(re_total.group())
        execution_mean[idx] = sum(execution_times) / iters

    t1 = execution_mean[0]
    metrics = [get_metrics(t1, tp, threads[p]) for p,tp in enumerate(execution_mean)]

    for m in metrics:
        print_dict(m)
        print()

    generate_graphics(metrics)

if __name__ == "__main__":
    main()
