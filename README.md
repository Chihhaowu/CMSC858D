# CMSC858D
Implementation of Bit Rank and Select, and Sparse Arrays

## Rank

The implementation of a succinct, constant time rank operation here considers an N-sized bit vector B that contain 1s at random indices, populated during its construction. A data structure  is created to support this rank operation; it consists of two tables, named Rs and Rb, which size is dependent on the length of the bit vector. To construct Rs, B is divided into superblocks, spanning s = ⌈log2(n)⌉2  number of non-overlapping positions of the bit vector. Excluding the first, each subsequent element in Rs is the rank of the last position in the previous superblock (alternatively consider this as the prefix sum); the first element is 0. Similarly, Rb divides the bit vector B into smaller blocks that span non-overlapping ranges of B. In this implementation, blocks cover √s = ⌈log2(n)⌉ positions such that the blocks and superblocks are guaranteed to be aligned. Again, each following element in Rb is the prefix sum of the bit vector up to the end of the previous block; however, for Rb the value resets to 0 at the start of the next superblock. Rank1 of an arbitrary index, here using 0-based indexing for queries, is by summing the values at positions in the vectors Rs, Rb, (indices [i/s] and [i/√s], respectively) and a value returned from an in-block query. The in-block query is simply the number of 1s present in the last block, from its start index up to the queried position.

## Select

The implementation of a worst-case, log-time, bit vector select operation uses rank and recursive binary search to efficiently find the index at which the ith 1-bit is present in the bit vector B[1…n]. Starting at the index midpoint of B, n/2, the rank is found. If the rank is greater than i, then the ith 1-bit is in the left half of the bit vector B[1…n/2], otherwise in B[n/2…n]. This search repeats until the program converges to the index where the ith 1-bit occurs. If the rank is i at index p, but the value at the p in the bit vector is 0, the program continues to search by first looking at B[1…p/2]. Select operations query using 1-based indexing.

## Sparse Array

The implementation of a sparse array that store a collection of strings, each associated with an index in the array uses bit vector rank and select. The values/strings are stored in a densely packed array. The following assumptions hold: new elements are inserted to the sparse array in-order, each index can correspond to only one element, and the index is less than the size of the sparse array. When a new element is inserted, it is appended to the densely packed array and a 1 is assigned to the specified index in the sparse array. Having constructed this sparse array, calling the rank(N), where N is the size of the sparse array, produces the number of elements. By calling the rank at an index with 1 in the sparse array, it is possible to look up the corresponding element.

https://rob-p.github.io/CMSC858D_S22/assignments/01_assignment_01
