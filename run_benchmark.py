import subprocess
import matplotlib.pyplot as plt
import csv
import numpy as np

# Function to run a command and return the execution time
def run_command(command):
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, timeout=30)
        if result.returncode != 0:
            print(f"Warning: Command '{command}' failed with error: {result.stderr}")
            return None
        output = result.stdout
        time_lines = [line for line in output.split('\n') if 'Duration' in line]
        if not time_lines:
            print(f"Warning: No timing information found in output of '{command}'")
            return None
        time_value = float(time_lines[0].split()[-2])
        return time_value
    except subprocess.TimeoutExpired:
        print(f"Warning: Command '{command}' timed out after 30 seconds")
        return None
    except Exception as e:
        print(f"Warning: Failed to run '{command}': {str(e)}")
        return None

# Number of runs
num_runs = 5

# Graph sizes to test
graph_sizes = [100, 1000, 10000, 20000, 1000000]

# CSV file to store results
csv_file = 'benchmark_results.csv'

# Colors for plotting
colors = {
    'Sequential': 'blue',
    'OpenMP': 'green',
    'MPI': 'red'
}

# Open CSV file for writing
with open(csv_file, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Graph Size', 'Implementation', 'Run', 'Time (seconds)'])

    # Collect timing data for each graph size
    for size in graph_sizes:
        graph_file = f"random_dag_{size}.txt"
        print(f"Running benchmarks for graph size: {size}")
        
        # Run benchmarks for each implementation
        sequential_times = [t for t in [run_command(f'./build/topsort_sequential {graph_file}') for _ in range(num_runs)] if t is not None]
        openmp_times = [t for t in [run_command(f'./build/topsort_openmp {graph_file}') for _ in range(num_runs)] if t is not None]
        mpi_times = [t for t in [run_command(f'mpirun -np 4 ./build/topsort_mpi {graph_file}') for _ in range(num_runs)] if t is not None]

        # Skip this size if we have no valid results
        if not sequential_times and not openmp_times and not mpi_times:
            print(f"Warning: No valid results for graph size {size}")
            continue

        # Write results to CSV
        for i, time in enumerate(sequential_times):
            writer.writerow([size, 'Sequential', i + 1, time])
        for i, time in enumerate(openmp_times):
            writer.writerow([size, 'OpenMP', i + 1, time])
        for i, time in enumerate(mpi_times):
            writer.writerow([size, 'MPI', i + 1, time])

        # Create histogram only if we have data
        if sequential_times or openmp_times or mpi_times:
            plt.figure(figsize=(10, 6))
            
            # Find valid min and max for binning
            all_times = sequential_times + openmp_times + mpi_times
            if all_times:
                bins = np.linspace(min(all_times), max(all_times), 20)
                
                if sequential_times:
                    plt.hist(sequential_times, bins=bins, alpha=0.5, label='Sequential', color=colors['Sequential'])
                if openmp_times:
                    plt.hist(openmp_times, bins=bins, alpha=0.5, label='OpenMP', color=colors['OpenMP'])
                if mpi_times:
                    plt.hist(mpi_times, bins=bins, alpha=0.5, label='MPI', color=colors['MPI'])
                
                plt.xlabel('Time (seconds)')
                plt.ylabel('Frequency')
                plt.title(f'Topological Sort Execution Time Distribution\nGraph Size: {size} Nodes')
                plt.legend(loc='upper right')
                plt.grid(True, alpha=0.3)
                plt.savefig(f'benchmark_histogram_{size}.png', dpi=300, bbox_inches='tight')
                plt.close()

    # Create summary plot
    plt.figure(figsize=(12, 6))
    
    # Calculate average times for each implementation and size
    implementations = ['Sequential', 'OpenMP', 'MPI']
    avg_times = {impl: [] for impl in implementations}
    std_times = {impl: [] for impl in implementations}
    valid_sizes = []
    
    with open(csv_file, mode='r') as f:
        reader = csv.DictReader(f)
        data = list(reader)
        
        for size in graph_sizes:
            has_valid_data = False
            for impl in implementations:
                times = [float(row['Time (seconds)']) 
                        for row in data 
                        if int(row['Graph Size']) == size and row['Implementation'] == impl]
                if times:
                    has_valid_data = True
                    avg_times[impl].append(np.mean(times))
                    std_times[impl].append(np.std(times))
                else:
                    avg_times[impl].append(np.nan)
                    std_times[impl].append(np.nan)
            if has_valid_data:
                valid_sizes.append(size)

    if valid_sizes:
        x = np.arange(len(valid_sizes))
        width = 0.25
        
        for i, impl in enumerate(implementations):
            valid_avgs = [avg for avg, size in zip(avg_times[impl], graph_sizes) if size in valid_sizes]
            valid_stds = [std for std, size in zip(std_times[impl], graph_sizes) if size in valid_sizes]
            
            if any(not np.isnan(avg) for avg in valid_avgs):
                plt.bar(x + i*width, valid_avgs, width, 
                        label=impl, color=colors[impl], alpha=0.7)
                plt.errorbar(x + i*width, valid_avgs, yerr=valid_stds,
                            fmt='none', color='black', capsize=5)

        plt.xlabel('Graph Size')
        plt.ylabel('Average Time (seconds)')
        plt.title('Comparison of Implementation Performance')
        plt.xticks(x + width, valid_sizes)
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Only use log scale if all values are positive
        if all(all(avg > 0 for avg in avgs if not np.isnan(avg)) for avgs in avg_times.values()):
            plt.yscale('log')
            
        plt.savefig('benchmark_summary.png', dpi=300, bbox_inches='tight')
        plt.close()

print(f"Results have been saved to {csv_file}")
print("Histograms and summary plot have been saved as PNG files") 