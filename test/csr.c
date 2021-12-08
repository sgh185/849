#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"

#define AI __attribute__((always_inline, annotate("analyze")))
#define ANALYZE __attribute__((annotate("analyze")))  

#define PERSIST 1
#define DEBUG 0
#define DEBUG_PRINT if (DEBUG) printf
#define DEBUG_ASSERT if (DEBUG) assert

/*
 * Take a matrix and convert it into compressed-row-storage CSR format
 *
 * The matrix has "unknown" dimensions --- they should be
 * parameters to the function. 
 *
 * The matrix is then converted to CSR and stored in a
 * csr struct and returned to the user.
 *
 * In this example, the parameters are hard coded as an
 * example of binary translation for the compiler-runtime
 * co-design.
 */ 

#define A_1 500
#define A_2 400


/*
 * CSR format
 */ 
typedef struct {
    int *non_zeros ;
    int *col_ind_for_non_zeros;
    int *row_starts;
} csr ;


AI
void convert_to_csr(
    int dim_A_1,
    int dim_A_2,
    int A[dim_A_1][dim_A_2],
    csr *result
)
{
    /*
     * Set up @result
     */ 
    int num_non_zeros = 0;
    int num_row_starts = 0;
    for (int i = 0 ; i < dim_A_1 ; i++)
    {
        bool row_start_found_among_non_zeros = false;
        for (int j = 0 ; j < dim_A_2 ; j++)
        {
            if (A[i][j] != 0)
            {
                num_non_zeros++;
                if (!row_start_found_among_non_zeros)
                {
                    num_row_starts++;
                    row_start_found_among_non_zeros = true;
                }
            }
        }
    }

    result->non_zeros = (int *) malloc(num_non_zeros * sizeof(int));
    result->col_ind_for_non_zeros = (int *) malloc(num_non_zeros * sizeof(int));
    result->row_starts = (int *) malloc(num_row_starts * sizeof(int));


    /*
     * Convert to CSR format
     */ 
    int next_non_zero_idx = 0;
    int next_row_start_idx = 0;
    for (int i = 0 ; i < dim_A_1 ; i++)
    {
        bool row_start_found_among_non_zeros = false;
        for (int j = 0 ; j < dim_A_2 ; j++)
        {
            if (A[i][j] != 0)
            {
                result->non_zeros[next_non_zero_idx] = A[i][j];
                result->col_ind_for_non_zeros[next_non_zero_idx] = j;
                
                if (!row_start_found_among_non_zeros)
                {
                    result->row_starts[next_row_start_idx] = next_non_zero_idx;
                    next_row_start_idx++;
                    row_start_found_among_non_zeros = true;
                }

                next_non_zero_idx++;
            }
        }
    }


    return;
}


ANALYZE
int main(void)
{
    /*
     * "Inputs"
     */ 
    int A[A_1][A_2];


    /*
     * Randomly init matrix
     */ 
    random_init_matrix(A_1, A_2, A);  


    /*
     * Convert and return the resulting matrix in CSR form
     */ 
    csr result;
    convert_to_csr(
        A_1, A_2,
        A, &result
    );


#if PERSIST
    printf("r %d\n", result.col_ind_for_non_zeros[0]);
    printf("r %d\n", result.non_zeros[0]);
    printf("r %d\n", result.row_starts[0]);
#endif


    return 0;
}



