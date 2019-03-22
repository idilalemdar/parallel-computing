#!/bin/bash

# set the partition where the job will run
#SBATCH --partition=halley

# The following line defines the name of the submitted job
#SBATCH --job-name=slurm_test

# The default output file if we run the script with the command sbatch test.sh
#SBATCH --output=1_out.txt

#SBATCH --nodes=1

mpiexec -n 1 ./dotproduct

