#! /bin/bash
set -e

module load gcc91
module load mpich-3.2.1--gcc-9.1.0

gcc() {
    gcc-9.1.0 "$@"
}
gcc --version
mpicc --version

# To check the architecture
lscpu

# Select the working directory
cd /home/$USER/HW3

mkdir -p out

mpicc src/matT.c -o out/matT.out -lm
mpicc src/matT_blocks.c -o out/matT_blocks.out -lm

qsub src/hw3_np1.pbs
qsub src/hw3_np4.pbs
qsub src/hw3_np16.pbs
qsub src/hw3_np64.pbs

watch qstat -u $USER
