echo "input,cores,run1,run2,run3" > runtime_parallel1.csv

for inputSize in 30000 100000
do
    for cores in 192 188 184 180 176 172 168 164 160 156 152 148 144 140 136 132 128 124 120 116 112 108 104 100 96 92 88 84 80 76 72 68 64 60 56 52 48 44 40 36 32 28 24 20 16 12 8 4 2 1
    do
        echo -n $inputSize >> runtime_parallel1.csv
        echo -n "," >> runtime_parallel1.csv
        echo -n $cores >> runtime_parallel1.csv
        for k in 1 2 3
        do
            echo -n "," >> runtime_parallel1.csv
            printf "%s" "$(srun --partition=amd-longq --mpi=pmi2 -N 3 -n $cores ./euler_totient_mpi $inputSize)" >> runtime_parallel1.csv
        done
        echo "" >> runtime_parallel1.csv
    done
done