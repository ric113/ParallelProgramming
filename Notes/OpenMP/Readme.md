# OpenMP

## Contents
* [Introduction](#intro)
* [Hello World in OpenMP])(#hwop)


## Introduction <a name="intro"></a>
* Open specification for Multi-Processor
* __Standard__ API for defining __multi-threaded shared-memory__ programs
* High Level API (相較於pthread)
    - Preprocessor directives (80%)
        - creating teams of threads
        - sharing work among threads
        - sync. the threads
    - Library Calls (19%)
        - setting and querying thread's attributes
    - Environment variables (1%)
        - controlling run-time behavior of the parallel program
* Fork-Join Parallelism
    - Master thread spawns a team of threads as needed
    - Parallelism added incrementally until performance are met
    ![](model.png)

## Hello World in OpenMP <a name="hwop"></a>
![](hwop.png)
* Compile
``` Shell
$ gcc -fopenmp -o main main.c
```