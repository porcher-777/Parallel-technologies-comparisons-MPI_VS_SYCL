import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Define function to plot speedups
def plot_speedups(sequential, parallel, title):
    # Calculate speedup for each input size
    speedups = sequential['Median'] / parallel['Median']

    plt.plot(sequential['Input'], speedups, marker='o', label='SYCL')
    plt.xlabel('Input Size')
    plt.ylabel('Speedup')
    plt.title(title)
    plt.legend()


# Function to plot bar graph for WG size configurations and compute speedup
def plot_wg_size_performance(gpuv3_data, title):
    # Create a bar plot
    plt.bar(gpuv3_data['WorkGroup'], gpuv3_data['Median'], color='blue', alpha=1, width=2, label='Version 3 (WG Sizes)')
    #plt.bar(gpuv2_data['WorkGroup'], gpuv3_data['Median'], color='red', alpha=1, width=0.2, label='Version 2')

    # Add labels and title
    plt.xlabel('Workgroup Size (WG Size)')
    plt.ylabel('Median Average Runtime (milliseconds)')
    plt.title(title)
    plt.xticks(gpuv3_data['WorkGroup'].unique())

    plt.legend()
    plt.grid(True)


# Function to plot grouped bar graph for CPU vs GPU runtimes for DS1, DS2, DS3
def plot_grouped_bars(cpu_data, gpu_data, title):
    cpu_data['Median'] = cpu_data.iloc[:, 1:].median(axis=1)
    gpu_data['Median'] = gpu_data.iloc[:, 1:].median(axis=1)

    # Extract input sizes
    input_sizes = cpu_data['Input']  # Assuming both files have the same input sizes

    # Extract median runtimes
    cpu_medians = cpu_data['Median']
    gpu_medians = gpu_data['Median']

    # Define bar width and positions
    x = np.arange(len(input_sizes))  # Position for groups
    width = 0.35  # Width of the bars

    # Create grouped bar plot
    fig, ax = plt.subplots(figsize=(10, 6))

    bar_cpu = ax.bar(x - width / 2, cpu_medians, width, label='CPU', color='blue')
    bar_gpu = ax.bar(x + width / 2, gpu_medians, width, label='GPU', color='orange')

    # Add labels and titles
    ax.set_xlabel('Input Size')
    ax.set_ylabel('Median Runtime (ms)')
    ax.set_title('Grouped Bar Graph for CPU vs GPU Runtimes')
    ax.set_xticks(x)
    ax.set_xticklabels(input_sizes)
    ax.legend()

    # Display the runtime values on top of the bars
    ax.bar_label(bar_cpu, padding=3)
    ax.bar_label(bar_gpu, padding=3)

    plt.grid(True)
    plt.savefig("cpu_vs_gpu_runtimes.png")
    plt.show()

# Load data from CSV files
sequential_data = pd.read_csv('sycl/build/runtimeSeq.csv', delimiter=',')
gpuv2_data = pd.read_csv('sycl/build/runtimeGPUv2.csv', delimiter=',')
gpuv3_data = pd.read_csv('sycl/build/runtimeGPUv3.csv', delimiter=',')  # Add GPUv3 data here
cpuv2_data = pd.read_csv('sycl/build/runtimeCPUv2.csv', delimiter=',')

# Plot speedups
"""plt.figure(figsize=(16, 6))
plot_speedups(sequential_data, gpuv2_data, "Speedup for the second SYCL version : reduction variable")
plt.grid(True)
plt.savefig("speedupV2.png")
plt.show()

# Plot WG size performance
plt.figure(figsize=(10, 6))
plot_wg_size_performance(gpuv3_data, "Performance of Version 3 with Different WG Sizes")
plt.grid(True)
plt.savefig("wg_size_performance.png")
plt.show()"""

# Plot Grouped Bar Graph for CPU vs GPU runtimes
plot_grouped_bars(cpuv2_data, gpuv2_data, "Grouped Bar Graph for CPU vs GPU Runtimes")