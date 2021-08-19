/* 
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
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, v1, v2, v3, v4, v5, v6, v7, v8;
    switch (M)
    {
    case 32: /* M=32, N=32 */
        for (i = 0; i < N - 7; i += 8)
            for (j = 0; j < M - 7; j += 8)
                for (int k = 0; k < 8; k++)
                {
                    v1 = A[i + k][j],
                    v2 = A[i + k][j + 1],
                    v3 = A[i + k][j + 2],
                    v4 = A[i + k][j + 3],
                    v5 = A[i + k][j + 4],
                    v6 = A[i + k][j + 5],
                    v7 = A[i + k][j + 6],
                    v8 = A[i + k][j + 7];

                    B[j][i + k] = v1,
                    B[j + 1][i + k] = v2,
                    B[j + 2][i + k] = v3,
                    B[j + 3][i + k] = v4,
                    B[j + 4][i + k] = v5,
                    B[j + 5][i + k] = v6,
                    B[j + 6][i + k] = v7,
                    B[j + 7][i + k] = v8;
                }
        break;
    case 64: /* M=64, N=64 */
        for (i = 0; i < N - 3; i += 4)
            for (j = 0; j < M - 3; j += 4)
                for (int k = 0; k < 4; k += 2)
                {
                    v1 = A[i + k][j],
                    v2 = A[i + k][j + 1],
                    v3 = A[i + k][j + 2],
                    v4 = A[i + k][j + 3],
                    v5 = A[i + k + 1][j],
                    v6 = A[i + k + 1][j + 1],
                    v7 = A[i + k + 1][j + 2],
                    v8 = A[i + k + 1][j + 3];

                    B[j][i + k] = v1,
                    B[j + 1][i + k] = v2,
                    B[j + 2][i + k] = v3,
                    B[j + 3][i + k] = v4,
                    B[j][i + k + 1] = v5,
                    B[j + 1][i + k + 1] = v6,
                    B[j + 2][i + k + 1] = v7,
                    B[j + 3][i + k + 1] = v8;
                }
        break;
    case 61: /* M=61, N=67 */
        for (i = 0; i < N - 15; i += 16)
            for (j = 0; j < M - 15; j += 16)
                for (int k = 0; k < 16; k++)
                {
                    v1 = A[i + k][j],
                    v2 = A[i + k][j + 1],
                    v3 = A[i + k][j + 2],
                    v4 = A[i + k][j + 3],
                    v5 = A[i + k][j + 4],
                    v6 = A[i + k][j + 5],
                    v7 = A[i + k][j + 6],
                    v8 = A[i + k][j + 7];

                    B[j][i + k] = v1,
                    B[j + 1][i + k] = v2,
                    B[j + 2][i + k] = v3,
                    B[j + 3][i + k] = v4,
                    B[j + 4][i + k] = v5,
                    B[j + 5][i + k] = v6,
                    B[j + 6][i + k] = v7,
                    B[j + 7][i + k] = v8;

                    v1 = A[i + k][j + 8],
                    v2 = A[i + k][j + 1 + 8],
                    v3 = A[i + k][j + 2 + 8],
                    v4 = A[i + k][j + 3 + 8],
                    v5 = A[i + k][j + 4 + 8],
                    v6 = A[i + k][j + 5 + 8],
                    v7 = A[i + k][j + 6 + 8],
                    v8 = A[i + k][j + 7 + 8];

                    B[j + 8][i + k] = v1,
                    B[j + 8 + 1][i + k] = v2,
                    B[j + 8 + 2][i + k] = v3,
                    B[j + 8 + 3][i + k] = v4,
                    B[j + 8 + 4][i + k] = v5,
                    B[j + 8 + 5][i + k] = v6,
                    B[j + 8 + 6][i + k] = v7,
                    B[j + 8 + 7][i + k] = v8;
                }
        for (int ii = 0; ii < i; ii++)
            for (int jj = j; jj < M; jj++)
                B[jj][ii] = A[ii][jj];
        for (; i < N; i++)
            for (j = 0; j < M; j++)
                B[j][i] = A[i][j];
        break;
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

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

