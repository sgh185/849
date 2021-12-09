#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"

#define AI __attribute__((always_inline, annotate("analyze")))
#define ANALYZE __attribute__((annotate("analyze")))  

#define PERSIST 1
#define DEBUG 0
#define DEBUG_PRINT if (DEBUG) printf
#define DEBUG_ASSERT if (DEBUG) assert

/*
 * Take two matrices and multiply them.
 *
 * Matrices have "unknown" dimensions --- they should be
 * parameters to the function. The function decides if 
 * multiplication is possible or not.
 *
 * In this example, the parameters are hard coded as an
 * example of binary translation for the compiler-runtime
 * co-design.
 */ 

#define A_1 60
#define A_2 30
#define B_1 30
#define B_2 120

AI
int **multiply(
    int dim_A_1,
    int dim_A_2,
    int dim_B_1,
    int dim_B_2,
    int A[dim_A_1][dim_A_2],
    int B[dim_B_1][dim_B_2]
)
{
    /*
     * Confirm that A and B can multiply 
     */ 
    int **result = NULL;
    if (dim_A_2 != dim_B_1) return result;


    /*
     * Allocate resulting matrix
     */ 
    result = (int **) malloc(dim_A_1 * sizeof(int *));
    for (int i = 0 ; i < dim_A_1 ; i++)
        result[i] = (int *) malloc(dim_B_2 * sizeof(int));


    /*
     * Multiply
     */
    for (int i = 0 ; i < dim_A_1 ; i++)
        for (int j = 0 ; j < dim_B_2 ; j++)
            for (int k = 0 ; k < dim_B_1 ; k++)
                result[i][j] += (A[i][k] * B[k][j]);


    return result;
}


ANALYZE
int main(void)
{
    /*
     * "Inputs"
     */ 
    int A[A_1][A_2];
    int B[B_1][B_2];


    /*
     * Randomly init matrix
     */
    random_init_matrix(A_1, A_2, A);
    random_init_matrix(B_1, B_2, B);


    /*
     * Multiply and return the resulting matrix
     */ 
    int **result = 
        multiply(
            A_1, A_2,
            B_1, B_2,
            A, B
        );


#if PERSIST
    for (int i = 0 ; i < A_1 ; i++)
        for (int j = 0 ; j < B_2 ; j++)
            printf("i, j = %d, %d : %d\n", i, j, result[i][j]); 

    printf("\n");
#endif


    return 0;
}



