#include <stdlib.h>
#include <stdio.h>

#define SIZE 32
#define ANALYZE __attribute__((annotate("analyze")))

ANALYZE
int main(void)
{
    int *arr = (int *) malloc(32);
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    free(arr);

    return 0;
}


ANALYZE
void dyn(int n)
{
    int *arr = (int *) malloc(n);
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    free(arr);

    return;

}


ANALYZE
void stack_with_arg(int n)
{
    int arr[n]; 
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    
    return;
}


ANALYZE
void stack(void)
{
    int arr[30]; 
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    
    return;
}
