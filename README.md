# Cache-Lab

Overview

This project is divided into two parts:

Part A: Cache Simulator

Implemented a cache simulator in C that processes Valgrind-generated memory trace files and mimics the behavior of an LRU (Least Recently Used) cache.

Part B: Optimized Matrix Transpose

Wrote a cache-efficient version of a matrix transpose function to minimize cache misses on a simulated 1KB direct-mapped cache with a 32-byte block size.


**Part A: Cache Simulator**

Files
* csim.c: Source code for the cache simulator.
* cachelab.h: Provided by the instructors to interface with the autograder.
* Makefile: Provided; compiles the simulator and test code.

Key Features
* Simulates an s-bit indexed, E-way associative, b-bit block offset cache.
* Uses LRU replacement policy via a timestamp (oldest) counter.
* Tracks and reports:
  * Cache hits
  * Cache misses
  * Evictions

Usage:
~~~~~~~~~~~~~~~~~~~
./csim -s <s> -E <E> -b <b> -t <tracefile> [-v]
~~~~~~~~~~~~~~~~~~~
Example:
~~~~~~~~~~~~~~~~~~~
./csim -s 4 -E 1 -b 4 -t traces/yi.trace -v
~~~~~~~~~~~~~~~~~~~

Command-Line Flags:
~~~~~~~~~~~~~~~~~~~
-s <s>: Number of set index bits
-E <E>: Number of lines per set (associativity)
-b <b>: Number of block bits
-t <tracefile>: Valgrind memory trace to simulate
-v: (Verbose) Prints hit/miss/eviction messages for each memory access
~~~~~~~~~~~~~~~~~~~


Implementation Notes
* Skips instruction load (I) accesses.
* Treats modify (M) accesses as a load followed by a store (two accesses).
* Uses a 2D array of Line structs to represent the cache.
* Each cache line tracks:
  * Valid bit
  * Tag
  * Age counter for LRU tracking

**Part B: Matrix Transpose**

File
* trans.c: Contains all matrix transpose logic and submission code.

Function to Be Graded
* transpose_submit(int M, int N, int A[N][M], int B[M][N])

Optimizations for 64×64 Matrix
* The matrix is divided into 8×8 blocks.
* Each 8×8 block is split into 4×4 sub-blocks to exploit spatial locality.
* The transpose avoids cache conflict misses by carefully storing and retrieving diagonal elements.
* Temporary variables (tmp0 to tmp7) are used to reduce memory accesses and reuse registers.

Cache Configuration (for Part B Grading)
* 1KB direct-mapped cache
* 32-byte block size

**Results**
Cache Simulator (Part A)
* Successfully compiled and simulated traces.
* Reports correct number of hits, misses, and evictions per trace file.

~~~~~~~~~~~~~~~~~
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27

TEST_CSIM_RESULTS=27
~~~~~~~~~~~~~~~~~

Matrix Transpose (Part B)
* Significantly reduced cache misses for:
  * 32×32
  * 64×64
  * 61×67 matrices
* Passed correctness check via is_transpose.

Build Instructions
~~~~~~~~~~~~~~~~~~~
make clean
make
~~~~~~~~~~~~~~~~~~~

To test:
~~~~~~~~~~~~~~~~~~~
./test-trans
~~~~~~~~~~~~~~~~~~~

Acknowledgments
* Cache simulator logic inspired by LRU replacement policy.
* Transpose strategies inspired by blocking and register reuse techniques common in cache-aware computing.
