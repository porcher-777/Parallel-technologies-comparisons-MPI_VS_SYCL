# If you are collecting data for the coursework, you should run this
# script on the head node on the cluster.
echo "Input,WorkGroup,Run1,Run2,Run3,Run4,Run5" > runtimeCPUv3.csv

for workGroup in 4 8 16 32 64 128 160
do
    echo -n "400000," >> runtimeCPUv3.csv
    echo -n $workGroup >> runtimeCPUv3.csv
    for k in 1 2 3 4 5
    do
        echo -n "," >> runtimeCPUv3.csv
        printf "%s" "$(COMPUTECPP_TARGET=host srun --cpus-per-task 64 --partition=amd-longq ./syclTotient 1 400000 $workGroup)" >> runtimeCPUv3.csv
    done
    echo "" >> runtimeCPUv3.csv
done

