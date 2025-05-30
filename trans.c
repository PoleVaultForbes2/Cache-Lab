/* 
 *      HEADER: 
 *      Name: Josh Forbes
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

/* MY CODE HERE */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    //tmp for transfer, block for diving matrix, diag for edge case of diagnoal when x == y
    int i, j, x, y;

    if(M == 64){
        //we are going to split the mattrix into 8x8 blocks and inside those blocks split them into 4x4 blocks
        //that way we can save the ones close by together without stretching rows to widely 
        // we will move the upper left into place, then save the upper right to bottom left which isn't be used
        int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        for(i = 0; i < 64; i += 8){
            for(j = 0; j < 64; j += 8){
                 for(x = 0; x < 4; x++){
                    //save upper row in temp
                    tmp0 = A[i+x][j+0];
                    tmp1 = A[i+x][j+1];
                    tmp2 = A[i+x][j+ 2];
                    tmp3 = A[i+x][j+3];
                    tmp4 = A[i+x][j+4];
                    tmp5 = A[i+x][j+5];
                    tmp6 = A[i+x][j+6];
                    tmp7 = A[i+x][j+7];

                    //save the upper left to correct B
                    B[j+0][i+x] = tmp0;
                    B[j+1][i+x] = tmp1;
                    B[j+2][i+x] = tmp2;
                    B[j+3][i+x] = tmp3;

                    //store right half in right side of B; this will go in bottom left
                    B[j+0][i+x+4] = tmp4;
                    B[j+1][i+x+4] = tmp5;
                    B[j+2][i+x+4] = tmp6;
                    B[j+3][i+x+4] = tmp7;
                }

                //now deal with right half of upper into B (upper right first goes to bottom left then put upper right in place)
                for(y = 0; y < 4; y++){
                    tmp0 = A[i+4][j+y];
                    tmp1 = A[i+5][j+y];
                    tmp2 = A[i+6][j+y];
                    tmp3 = A[i+7][j+y];

                    tmp4 = B[j+y][i+4];
                    tmp5 = B[j+y][i+5];
                    tmp6 = B[j+y][i+6];
                    tmp7 = B[j+y][i+7];

                    B[j+y][i+4] = tmp0;
                    B[j+y][i+5] = tmp1;
                    B[j+y][i+6] = tmp2;
                    B[j+y][i+7] = tmp3;

                    B[j+y+4][i+0] = tmp4;
                    B[j+y+4][i+1] = tmp5;
                    B[j+y+4][i+2] = tmp6;
                    B[j+y+4][i+3] = tmp7;
                }

                //now do the lower-right block directly
                for(x = 4; x < 8; x++){
                    B[j+4][i+x] = A[i+x][j+4];
                    B[j+5][i+x] = A[i+x][j+5];
                    B[j+6][i+x] = A[i+x][j+6];
                    B[j+7][i+x] = A[i+x][j+7];
                }
            }
        }

    }
    else if (M == 32){
        //use blocks to process it in small chunks instead of all at once
        for(i = 0; i < N; i += 8){
            for(j = 0; j < M; j += 8){
                for(x = i; x < i + 8; x++){
                    for(y = j; y < j + 8; y++){
                        B[y][x] = A[x][y];
                    }
                }
            }
        }
    }else{
        for(i = 0; i < N; i += 16){
            for(j = 0; j < M; j += 16){
                //add checks for the 61x67 to make sure it is not out of bounds
                for(x = i; x < i + 16 && x < N; x++){
                    for(y = j; y < j + 16 && y < M; y++){
                        B[y][x] = A[x][y];
                    }
                }
            }
        }
    }
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
