import subprocess
import matplotlib.pyplot as plt
import csv

# Function to run a command and return the execution time
def run_command(command):
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    output = result.stdout
    time_line = [line for line in output.split('\n') if 'Duration' in line][0]
    time_value = float(time_line.split()[-2])
    return time_value

# Number of runs
num_runs = 5

# Graph sizes to test
graph_sizes = [100, 1000, 10000, 20000]

# CSV file to store results
csv_file = 'benchmark_results.csv'

# Open CSV file for writing
with open(csv_file, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Graph Size', 'Implementation', 'Run', 'Time (seconds)'])

    # Collect timing data for each graph size
    for size in graph_sizes:
        graph_file = f"random_dag_{size}.txt"
        print(f"Running benchmarks for graph size: {size}")
        openmp_times = [run_command(f'./build/topsort_openmp {graph_file}') for _ in range(num_runs)]
        mpi_times = [run_command(f'mpirun -np 4 ./build/topsort_mpi {graph_file}') for _ in range(num_runs)]

        # Write results to CSV
        for i, time in enumerate(openmp_times):
            writer.writerow([size, 'OpenMP', i + 1, time])
        for i, time in enumerate(mpi_times):
            writer.writerow([size, 'MPI', i + 1, time])

        # Plot and save histogram
        plt.hist(openmp_times, alpha=0.5, label='OpenMP')
        plt.hist(mpi_times, alpha=0.5, label='MPI')
        plt.xlabel('Time (seconds)')
        plt.ylabel('Frequency')
        plt.title(f'Topological Sort Execution Time for {size} Nodes')
        plt.legend(loc='upper right')
        plt.savefig(f'benchmark_histogram_{size}.png')
        plt.clf()  # Clear the figure for the next plot 