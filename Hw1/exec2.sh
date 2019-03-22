#!/bin/bash

# set the partition where the job will run
#SBATCH --partition=halley

# The following line defines the name of the submitted job
#SBATCH --job-name=slurm_test

# The default output file if we run the script with the command sbatch test.sh
#SBATCH --output=2_out.txt

#SBATCH --nodes=2

mpiexec -n 2 ./dotproduct

