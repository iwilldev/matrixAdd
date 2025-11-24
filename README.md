# matrixAdd
Just a quick multithread matrix addition algorithm I made for an Operating Systems class. Has nice looking code.

This program accepts 2 arguments, *matrix size* and *thread count* with a limit of 10000 x 10000 for *matrix size* and a limit of 4 threads.


### Example:
	./matrixadd 10000 4



## Compilation

matrixAdd can be compiled using the **make** command. To clean the directory before building, use **make clean**.

## Performance
The results were tracked on a system running **Fedora Linux 42**, with an **Intel Core i5 14500**

matrixAdd Execution timings:
  
  1000x1000 matrix
    
    1 thread  - 4.7600 miliseconds
    2 threads - 3.1480 miliseconds
    4 threads - 2.6510 miliseconds
    
  5000x5000 matrix
  
    1 thread  - 82.2320 miliseconds
    2 threads - 46.2360 miliseconds
    4 threads - 30.9930 miliseconds
  
  10000x10000 matrix
    
    1 thread  - 298.3340 miliseconds
    2 threads - 167.2709 miliseconds
    4 threads - 107.8420 miliseconds

## Releases
[Here](https://github.com/iwilldev/matrixAdd/releases) you can find the releases page, where a compiled binary is available.