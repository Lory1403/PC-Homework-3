# PC-Homework-3
Introduction to Parallel Computing - Homework 3: Parallelizing matrix operations using MPI

This repository containes the solutions I have developed for the first assignment of the course ***"Introduction to Parallel Computing [146209] - Prof. Vella"*** about ***"Parallelizing matrix operations using MPI"*** 

## Execution
> [!IMPORTANT] 
> **HPC Cluster**: This guide is intended only to be executed on the HPC Cluster

To execute the code, follow these steps:  
1. Open your terminal.

2. Clone the repository of the personal folder on the Cluster.
   
4. Run the following command to run the script: `./run.sh`.

Execution example:
```shell
git clone https://https://github.com/Lory1403/PC-Homework-3 ~/HW3
cd ~/HW3
./run.sh
```

The script will executes the necessary commands/tasks to prepare the environment and then submits the simulations jobs to the cluster. \
The user will be then redirected to the monitor for the active user's jobs (`CTRL+C` to quit).

## Results
At the end of the job, the results of the computations will be available in `matT_np<num>.o` files in the `out/` folder.
Errors abort the execution of the job immediately and the `stderr` is dumped into `out/matT_np<num>.e`. \
The report analyzing the results is available [here](report/build/report.pdf).
