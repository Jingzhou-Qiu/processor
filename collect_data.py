import subprocess
import matplotlib.pyplot as plt

# List of your input filenames

branch = [
"branch", "static_branch", "branch_prediction_1", "branch_prediction_2",
"branch_prediction_3", "one_forward_beq", "one_forward_bne", "one_backward_beq",
"one_backward_bne", "3bit_011", "3bit_100", "3bit_double", "mips_hidden", "hidden", "test_case_local"
]

superscalar = [
    "simple_no_branch_no_dep", "superscalar_R10000"
]


dependencies = [
    "dependent_stores", "false_dependency", "raw_dependency", "reg_file_fowarding",
    "forward_to_rs", "forward_to_rt", "load_use", "store_to_load_1", "store_to_load_2"
]


memory = [
    "memory_bound_L1", "memory_bound_L2", "memory_bound_memory", "memory_random", "unified_cache_1", "victim_cache",
    "instruction_prefetch_1", "instruction_prefetch_2", "value_prediction"
]

files = branch + superscalar + dependencies + memory






# Path prefix
path_prefix = "/u/yzp7fe/processor/compile/"

# Processor executable
optimized = "/u/yzp7fe/processor/mips_cpu/processor"
pipeline = "/u/yzp7fe/processor/pipeline/processor"


pipelineline_cycles = []
optimized_cycles = []


for file in files:
    input_path = path_prefix + file
    print("=== Running {} ===".format(file))
    
    optimized_result = subprocess.run(
        [optimized, "-b", input_path, "-O2"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True  # decode bytes to str automatically (works if Python 3.6+)
    )

    pipeline_result = subprocess.run(
        [pipeline, "-b", input_path, "-O1"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True  # decode bytes to str automatically (works if Python 3.6+)
    )
    
    print(optimized_result.stdout)
    print(pipeline_result.stdout)



    optimized_cycles.append(int(optimized_result.stdout.strip()))
    pipelineline_cycles.append(int(pipeline_result.stdout.strip()))


x = range(len(files))
width = 0.35

plt.figure(figsize=(20, 8))
plt.bar(x, pipelineline_cycles, width=width, label='pipeline', align='center')
plt.bar([i + width for i in x], optimized_cycles, width=width, label='optimized', align='center')

plt.xticks([i + width / 2 for i in x], files, rotation=90)
plt.ylabel("Cycle Count")
plt.title("Cycle Count Comparison per Test Case")
plt.legend()
plt.tight_layout()
plt.grid(axis='y')


plt.savefig("dependency_count.png", dpi=300)


speedup = [
    pipeline / opt if opt > 0 else 0
    for pipeline, opt in zip(pipelineline_cycles, optimized_cycles)
]
# Plot speedup
x = range(len(files))
plt.figure(figsize=(20, 6))
plt.bar(x, speedup, color='mediumseagreen')

plt.xticks(x, files, rotation=90)
plt.ylabel("Speedup (Pipeline / Optimized)")
plt.title("Performance Improvement per Test Case (Amdahl’s Law Perspective)")
plt.axhline(y=1.0, color='red', linestyle='--', linewidth=1, label="No Speedup")
plt.legend()
plt.grid(axis='y')
plt.tight_layout()

# Save figure
plt.savefig("dependency_speedup.png", dpi=300)
