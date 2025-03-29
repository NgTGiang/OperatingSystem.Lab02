/*
Given the following function:
    sum(n) = 1+2+...+n
This is the sum of a large set including n numbers from 1 to n. If n is a large number, this will take
a long time to calculate the sum(n). The solution is to divide this large set into pieces and calculate the
sum of these pieces concurrently by using threads. Suppose the number of threads is numThreads, so
the 1st thread calculates the sum of {1,n/ numThreads }, the 2 nd thread carries out the sum of {n/
numThreads +1,2n/ numThreads },...

Write two programs implementing algorithm describe above: one serial version and one multi-thread version.
The program takes the number of threads and n from user then creates multiple threads to calculate
the sum. Put all of your code in two files named ”sum serial.c” and ”sum multi-thread.c”. The number of
threads and n are passed to your program as an input parameter. For example, you will use the following
command to run your program for calculating the sum of 1M :
    $./ sum_serial 1000000
    $./ sum_multi_thread 10 1000000
    (#numThreads=10)
    
Requirement: The multi-thread version may improve speed-up compared to the serial version. There
are at least 2 targets in the Makefile sum serial and sum multi-thread to compile the two program.
*/