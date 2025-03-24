# If you are collecting data for the coursework, you should run this
# script on the head node on the cluster.
echo "Input,Run1,Run2,Run3,Run4,Run5" > runtimeSeq1.csv

for inputSize in 400000
do
    echo -n $inputSize >> runtimeSeq1.csv
    for k in 1 2 3 4 5
    do
        echo -n "," >> runtimeSeq1.csv
        printf "%s" "$(COMPUTECPP_TARGET=host srun --cpus-per-task 64 --partition=amd-longq ./syclTotient 1 $inputSize)" >> runtimeSeq1.csv
    done
    echo "" >> runtimeSeq1.csv
done
