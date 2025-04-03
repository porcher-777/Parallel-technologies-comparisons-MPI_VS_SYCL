echo "input,cores,run1,run2,run3" > runtime_parallel.csv

for inputSize in 15000 30000 100000
do
    for cores in 1 2 4 8 12 16 20 24 28 32 36 40 44 48 52 56 60 64 68 72 76 80 84 88 92 96 100 104 108 112 116 120 124 128 132 136 140 144 148 152 156 160 164 168 172 176 180 184 188 192
    do
        echo -n $inputSize >> runtime_parallel.csv
        echo -n "," >> runtime_parallel.csv
        echo -n $cores >> runtime_parallel.csv
        for k in 1 2 3
        do
            echo -n "," >> runtime_parallel.csv
            printf "%s" "$(srun --partition=amd-longq --mpi=pmi2 -N 3 -n $cores ./euler_totient_mpi $inputSize)" >> runtime_parallel.csv
        done
        echo "" >> runtime_parallel.csv
    done
done