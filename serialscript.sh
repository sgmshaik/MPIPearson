!/bin/bash
rm s_res.dat
gcc  -std=c99 -o serial.out pearsons.c -lm
for ((size = 2000000; size < 5000000; size+=1000000))
do
for ((i=1; i<20; i+=2))
do 
./serial.out $size
done
done