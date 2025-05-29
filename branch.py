import subprocess
import matplotlib.pyplot as plt

branch = [
    "branch", "static_branch", "branch_prediction_1", "branch_prediction_2",
    "branch_prediction_3", "one_forward_beq", "one_forward_bne", "one_backward_beq",
    "one_backward_bne", "3bit_011", "3bit_100", "3bit_double", "mips_hidden", "hidden", "test_case_local"
]

superscalar = ["simple_no_branch_no_dep", "superscalar_R10000"]

dependencies = [
    "dependent_stores", "false_dependency", "raw_dependency", "reg_file_fowarding",
    "forward_to_rs", "forward_to_rt", "load_use", "store_to_load_1", "store_to_load_2"
]

memory = [
    "memory_bound_L1", "memory_bound_L2", "memory_bound_memory", "memory_random", "unified_cache_1", "victim_cache",
    "instruction_prefetch_1", "instruction_prefetch_2", "value_prediction"
]

files = branch + superscalar + dependencies + memory
files = branch

path_prefix = "/u/yzp7fe/processor/compile/"
optimized = "/u/yzp7fe/processor/mips_cpu_1/processor"
pipeline = "/u/yzp7fe/processor/pipeline/processor"
no_branch_optimized = "/u/yzp7fe/processor/mips_cpu/processor"

pipeline_cycles, optimized_cycles, no_branch_cycles = [], [], []

for file in files:
    input_path = path_prefix + file

    
    pipe_result = subprocess.run([pipeline, "-b", input_path, "-O1"], stdout=subprocess.PIPE, text=True)
    opt_result = subprocess.run([optimized, "-b", input_path, "-O2"], stdout=subprocess.PIPE, text=True)
    no_branch_result = subprocess.run([no_branch_optimized, "-b", input_path, "-O2"], stdout=subprocess.PIPE, text=True)

    optimized_cycles.append(int(opt_result.stdout.strip()))
    pipeline_cycles.append(int(pipe_result.stdout.strip()))
    no_branch_cycles.append(int(no_branch_result.stdout.strip()))

# Plot cycle comparison
x = range(len(file))
width = 0.25
plt.figure(figsize=(24, 8))

plt.bar(x, pipeline_cycles, width, label='Pipeline', align='center')
plt.bar([i + width for i in x], no_branch_cycles, width, label='No Branch Optimized', align='center')
plt.bar([i + 2*width for i in x], optimized_cycles, width, label='Optimized', align='center')

plt.xticks([i + width for i in x], files, rotation=90)
plt.ylabel("Cycle Count")
plt.title("Cycle Count Comparison per Test Case")
plt.legend()
plt.grid(axis='y')
plt.tight_layout()
plt.savefig("branch_count.png", dpi=300)

# Calculate speedups
speedup_no_branch = [pipe / no_branch if no_branch else 0 for pipe, no_branch in zip(pipeline_cycles, no_branch_cycles)]
speedup_optimized = [pipe / opt if opt else 0 for pipe, opt in zip(pipeline_cycles, optimized_cycles)]
speedup_optimized = [no_branch / opt if opt else 0 for no_branch, opt in zip(no_branch_cycles, optimized_cycles)]

# Plot speedups
plt.figure(figsize=(24, 8))
# plt.bar(x, speedup_no_branch, width, label='No Branch vs Pipeline', color='skyblue')
plt.bar([i + width for i in x], speedup_optimized, width, label='Branch vs no Branch', color='mediumseagreen')

plt.axhline(y=1.0, color='red', linestyle='--', linewidth=1, label="No Speedup")
plt.xticks([i + width/2 for i in x], files, rotation=90)
plt.ylabel("Speedup")
plt.title("Speedup Comparison per Test Case")
plt.legend()
plt.grid(axis='y')
plt.tight_layout()
plt.savefig("branch_speedup.png", dpi=300)

