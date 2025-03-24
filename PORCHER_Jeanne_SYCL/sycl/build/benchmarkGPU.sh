# If you are collecting data for the coursework, you should run this
# script on the head node on the cluster.
echo "Input,WorkGroup,Run1,Run2,Run3,Run4,Run5" > runtimeGPUv3.csv

for workGroup in 4 8 16 32 64 128 160
do
    echo -n "400000," >> runtimeGPUv3.csv
    echo -n $workGroup >> runtimeGPUv3.csv
    for k in 1 2 3 4 5
    do
        echo -n "," >> runtimeGPUv3.csv
        printf "%s" "$(srun --partition=amd-longq ./syclTotient 1 400000 $workGroup)" >> runtimeGPUv3.csv
    done
    echo "" >> runtimeGPUv3.csv
done