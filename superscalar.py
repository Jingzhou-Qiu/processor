import subprocess
import matplotlib.pyplot as plt

# === Benchmark List ===
superscalar = ["simple_no_branch_no_dep", "superscalar_R10000"]
files = superscalar

# === Processor Executables ===
pipeline_exec = "/u/yzp7fe/processor/pipeline/processor"
way_1_exec = "/u/yzp7fe/processor/mips_cpu_1/processor"
way_2_exec = "/u/yzp7fe/processor/mips_cpu_2/processor"
way_4_exec = "/u/yzp7fe/processor/mips_cpu_4/processor"
way_8_exec = "/u/yzp7fe/processor/mips_cpu_8/processor"

# === Paths ===
compile_path = "/u/yzp7fe/processor/compile/"

# === Cycle Count Lists ===
pipeline_cycles = []
way_1_cycles = []
way_2_cycles = []
way_4_cycles = []
way_8_cycles = []

# === Helper Function ===
def run_and_get_cycles(proc_path, opt_flag, input_path):
    result = subprocess.run(
        [proc_path, "-b", input_path, opt_flag],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True
    )
    return int(result.stdout.strip())

# === Run Benchmarks ===
for file in files:
    input_path = compile_path + file
    print(f"Running {file}...")

    pipeline_cycles.append(run_and_get_cycles(pipeline_exec, "-O1", input_path))
    way_1_cycles.append(run_and_get_cycles(way_1_exec, "-O2", input_path))
    way_2_cycles.append(run_and_get_cycles(way_2_exec, "-O2", input_path))
    way_4_cycles.append(run_and_get_cycles(way_4_exec, "-O2", input_path))
    way_8_cycles.append(run_and_get_cycles(way_8_exec, "-O2", input_path))

# === Plot: Cycle Count Comparison (Bar Chart) ===
x = range(len(files))
bar_width = 0.15

plt.figure(figsize=(12, 5))
plt.bar([i - 2 * bar_width for i in x], pipeline_cycles, width=bar_width, label="Pipeline")
plt.bar([i - bar_width for i in x], way_1_cycles, width=bar_width, label="1-Way")
plt.bar([i for i in x], way_2_cycles, width=bar_width, label="2-Way")
plt.bar([i + bar_width for i in x], way_4_cycles, width=bar_width, label="4-Way")
plt.bar([i + 2 * bar_width for i in x], way_8_cycles, width=bar_width, label="8-Way")

plt.xticks([i for i in x], files)
plt.ylabel("Cycle Count")
plt.title("Cycle Count Comparison Across Superscalar Widths")
plt.legend()
plt.grid(axis='y')
plt.tight_layout()
plt.savefig("superscalar_count.png", dpi=300)
plt.show()

# === Compute Speedup ===
def compute_speedup(baseline, variant):
    return [baseline[i] / variant[i] if variant[i] else 0 for i in range(len(baseline))]

speedup_1 = compute_speedup(pipeline_cycles, way_1_cycles)
speedup_2 = compute_speedup(pipeline_cycles, way_2_cycles)
speedup_4 = compute_speedup(pipeline_cycles, way_4_cycles)
speedup_8 = compute_speedup(pipeline_cycles, way_8_cycles)

# === Plot: Speedup Comparison (Bar Chart) ===
plt.figure(figsize=(12, 5))
bar_positions = [i for i in x]
plt.bar([i - 1.5 * bar_width for i in x], speedup_1, width=bar_width, label="1-Way")
plt.bar([i - 0.5 * bar_width for i in x], speedup_2, width=bar_width, label="2-Way")
plt.bar([i + 0.5 * bar_width for i in x], speedup_4, width=bar_width, label="4-Way")
plt.bar([i + 1.5 * bar_width for i in x], speedup_8, width=bar_width, label="8-Way")

plt.xticks(bar_positions, files)
plt.ylabel("Speedup Over Pipeline")
plt.title("Superscalar Speedup Comparison (Bar Graph)")
plt.axhline(y=1.0, color='red', linestyle='--', linewidth=1, label="No Speedup")
plt.legend()
plt.grid(axis='y')
plt.tight_layout()
plt.savefig("superscalar_speedup.png", dpi=300)
plt.show()
