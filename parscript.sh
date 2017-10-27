!/bin/bash
rm p_res.dat
mpicc -g -std=c99 -o par.out pearsonparrallel.c -lm
for ((size = 2000000; size < 5000000; size+=1000000))
do
for ((i=1; i<20; i+=2))
do 
mpiexec -n $i ./par.out $size
done
done