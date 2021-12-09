#include <stdio.h>
#include <stdlib.h>
#include "parser.hpp"

/*
 * Based on OpenGenus/cosmos. See original: https://github.com/OpenGenus/cosmos/blob/6544834d1ed3c426c45d3738194fefaef1b10725/code/greedy_algorithms/src/huffman_coding/huffman_coding.c
 */ 

#define MAX_TREE_HT 100

#define AI __attribute__((always_inline, annotate("analyze")))
#define ANALYZE __attribute__((annotate("analyze")))  
#define PERSIST 1

struct MinHeapNode {
    char data;
    int freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    int size;
    int capacity;
    struct MinHeapNode** array;
};

AI
struct MinHeapNode* 
newNode(char data, int freq)
{
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
 
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
 
    return (temp);
}

AI
struct MinHeap* 
createMinHeap(int capacity) 
{
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
 
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));

    return (minHeap);
}

AI
void 
swapMinHeapNode(struct MinHeapNode** a,struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

ANALYZE
void 
minHeapify(struct MinHeap* minHeap, int index)
{
    int smallest = index;
    int left = (2 * index) + 1;
    int right = (2 * index) + 2;
 
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
 
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;
 
    if (smallest != index) {
        swapMinHeapNode(&minHeap->array[smallest],&minHeap->array[index]);
        minHeapify(minHeap, smallest);
    }
}

AI
int 
isSizeOne(struct MinHeap* minHeap)
{
    return (minHeap->size == 1);
}

AI
struct MinHeapNode* 
extractMin(struct MinHeap* minHeap)
{
    struct MinHeapNode* temp = minHeap->array[0];

    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
 
    return (temp);
}

AI
void 
insertMinHeap(struct MinHeap* minHeap,struct MinHeapNode* minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;
 
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
 
    minHeap->array[i] = minHeapNode;
}

AI
void
buildMinHeap(struct MinHeap* minHeap)
{
    int i;
    for (i = (minHeap->size - 2) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

ANALYZE
void 
printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
}

AI
int 
isLeaf(struct MinHeapNode* root)
{
    return !(root->left) && !(root->right);
}

AI
struct MinHeap*
createAndBuildMinHeap(char data[], int freq[], int size)
{
    struct MinHeap* minHeap = createMinHeap(size);
    
    int i;
    for (i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);
 
    return (minHeap);
}

AI
struct MinHeapNode* 
buildHuffmanTree(char data[], int freq[], int size) 
{
    struct MinHeapNode *left, *right, *top;

    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);
    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('@', left->freq + right->freq);
 
        top->left = left;
        top->right = right;
 
        insertMinHeap(minHeap, top);
    }
    return (extractMin(minHeap));
}

AI
void 
printCodes(struct MinHeapNode* root, int arr[], int top)
{
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (isLeaf(root)) { 
        printf("%c: ", root->data);
        printArr(arr, top);
    }
}

AI
void 
HuffmanCodes(char data[], int freq[], int size) 
{
    struct MinHeapNode* root= buildHuffmanTree(data, freq, size);

    int arr[MAX_TREE_HT], top = 0;
 
    printCodes(root, arr, top);
}

ANALYZE
int 
main()
{
    parse_package package = parse_text();
    int size = package.num_recorded;
    char *arr = package.chars;
    int *freq = package.freqs;
 
    HuffmanCodes(arr, freq, size);
 
    return 0;
}
