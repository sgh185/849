#pragma once

#include <stdint.h>
#include <stdint.h>

#define MOD 42

static inline uint64_t __attribute__((always_inline))
rdtsc (void)
{
    uint32_t lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return lo | ((uint64_t)(hi) << 32);
}


static inline void random_init_adj_matrix(
    int dim,
    int M[dim][dim]
)
{
    for (int i = 0 ; i < dim ; i++)
        for (int j = 0 ; j < dim ; j++)
            M[i][j] = 0;


    for (int i = 0 ; i < dim ; i++) 
    {
        for (int j = 0 ; j < dim ; j++) 
        {
            if (M[i][j] && M[j][i]) continue;
            if ((lrand48() % MOD) % 2) M[i][j] = M[j][i] = 1;
        }
    }


    return;
}


static inline void random_init_matrix(
    int rows,
    int cols,
    int M[rows][cols]
)
{
    for (int i = 0 ; i < rows ; i++)
        for (int j = 0 ; j < cols ; j++)
            M[i][j] = 0;


    for (int i = 0 ; i < rows ; i++) 
        for (int j = 0 ; j < cols ; j++)
            if ((lrand48() % MOD) % 2)
                M[i][j] = 1;


    return;
}



