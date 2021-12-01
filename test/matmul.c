#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

#define A_1 50
#define A_2 40
#define B_1 40
#define B_2 100


__attribute__((always_inline))
int **multiply(
    int **A,
    int **B,
    int dim_A_1,
    int dim_A_2,
    int dim_B_1,
    int dim_B_2
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
    result = (int **) malloc(dim_A_1 * sizeof(int));
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


int main(void)
{
    /*
     * "Inputs"
     */ 
    int A[A_1][A_2];
    int B[B_1][B_2];


    /*
     * Multiply and return the resulting matrix
     */ 
    int **result = 
        multiply(
            (int **) &A, (int **) &B,
            A_1, A_2,
            B_1, B_2
        );


    return 0;
}



