#include <stdlib.h>
#include <stdio.h>

#define SIZE 32

int main(void)
{
    int *arr = (int *) malloc(32);
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    free(arr);

    return 0;
}


void dyn(int n)
{
    int *arr = (int *) malloc(n);
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    free(arr);

    return;

}


void stack_with_arg(int n)
{
    int arr[n]; 
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    
    return;
}


void stack(void)
{
    int arr[30]; 
    arr[3] = 42;
    printf("%p : %d\n", arr + 3, arr[3]);
    
    return;
}
