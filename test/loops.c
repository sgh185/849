#include <stdlib.h>
#include <stdio.h>


#define WIDTH 8
#define VEC 0
#define NOUNROLL 0


/*
 * Sources:
 * - https://llvm.org/docs/Vectorizers.html
 */ 


void array_computation(
    float *A, 
    float *B, 
    float K, 
    int n
) 
{
#if NOUNROLL
    #pragma nounroll 
#endif
    for (int i = 0 ; i < n ; i++)
        A[i] *= B[i] + K;
}


void array_assignment(
    int *A, 
    int n
) 
{
#if VEC
    #pragma clang loop vectorize_width(WIDTH)
#endif
    for (int i = 0 ; i < n ; i++)
        A[i] = i;

    return;
}


void combine_arrays(
    int *a, 
    int *b, 
    int count
)
{
#if NOUNROLL
    #pragma nounroll 
#endif
    for (int i = 0 ; i < count ; i++)
        a[i] = b[i] + 1;
}


int reduction (
    int *A, 
    int n
) 
{
    unsigned sum = 0;

#if VEC
    #pragma clang loop vectorize_width(WIDTH)
#endif
    for (int i = 0 ; i < n ; ++i)
        sum += A[i] + 5;
    
    return sum;
}


int reduc (
    int *A, 
    int n
) 
{
    unsigned sum = 0;

    for (int i = 0 ; i < n ; ++i)
        sum += A[i] + 5;
    
    return sum;
}


#if 0

/*
 * Vectorized code (pseudo-C)
 */

void array_assignment(
    int *A, 
    int n
) 
{
    int strided_iters = n / 4 /* stride=4*/;

    /* Vectorized assignment */
    for (int i = 0, s = 0 ; s < strided_iters ; i += 4, s++)
        A[i ... i + 3] = {i .. i + 3};

    /* Singular assignment (for remainder) */
    for (; i < n ; i++)
        A[i] = i;

    return;
}

#endif


int main(void)
{
    return 0;
}
