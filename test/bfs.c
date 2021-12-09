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
 * Take a graph and two vertices -- compute BFS given a 
 * a source and return the traversal as a linked list
 *
 * The graph is a weighted undirected graph, where 
 * edges are represented by positive numbers and non-
 * edges are represented with 0.
 *
 * The graph input is represented as an adjacency 
 * matrix. This matrix should be an input to the
 * program, but is hard coded here as an example
 * of binary translation for the compiler-runtime
 * co-design
 */ 

#define NUM_VERTICES 1000


/*
 * Simple linked list implementation
 */ 
typedef struct node {
    struct node *next;
    int value;
} node;

typedef struct {
    struct node *front ;
    int size ;
} list ;


AI
void push(
    list *queue,
    int entry_to_add
)
{
    /*
     * Push @entry_to_add to @queue (add to front)
     */ 
    
    /*
     * Create a node for @entry_to_add 
     */ 
    node *next = 
        (queue->size == 0) ? 
        NULL : (queue->front) ;

    node *new_node = (node *) malloc(sizeof(node));
    new_node->next = next;
    new_node->value = entry_to_add;


    /*
     * Add the node to the list
     */ 
    queue->front = new_node;
    queue->size++;


    return;
}


AI
void pop(
    list *queue
)
{
    /*
     * Pop the last entry from @queue
     */
    
    /*
     * Do nothing if the queue is empty
     */ 
    if (queue->size == 0) return;


    /*
     * Pop the last entry (pop from front)
     */ 
    node *new_front = queue->front->next ;
    free(queue->front);
    queue->front = new_front;
    queue->size--;

    
    return;
}


AI
list bfs(
    int **graph,
    int num_vertices,
    int src
)
{
    /*
     * Perform BFS on @graph starting at @src and 
     * return the traversal as a linked list
     */ 

    /*
     * Initialize lists
     */ 
    list queue = {
        .front = NULL,
        .size = 0
    };

    list traversal = {
        .front = NULL,
        .size = 0
    } ;


    /*
     * Initialize visited set
     */ 
    bool *visited = (bool *) malloc(num_vertices * sizeof(bool));
    for (int i = 0 ; i < num_vertices ; i++) visited[i] = 0;


    /*
     * Initialize traversal
     */ 
    push(&queue, src);
    visited[src] = true;


    /*
     * Begin traversal
     */ 
    while (queue.size)
    {
        /*
         * Fetch the next entry
         */ 
        int next = queue.front->value;
        pop(&queue);
        push(&traversal, next);


        /*
         * Walk through neighbors, iterator is the vertex ID
         */ 
        for (int i = 0 ; i < num_vertices ; i++)
        {
            if (visited[i]) continue;
            visited[i] = true;
            push(&queue, i);
        }
    }


    /*
     * Cleanup
     */ 
    while (queue.size) pop(&queue);


    return traversal ;
}


ANALYZE
int main(void)
{
    /*
     * "Inputs"
     */ 
    int graph[NUM_VERTICES][NUM_VERTICES];
    int src = 42;


    /*
     * Randomly fill out graph
     */ 
    random_init_adj_matrix(NUM_VERTICES, graph); 
    

    /*
     * Perform BFS traversal, get the result
     */ 
    list traversal_result = 
        bfs(
            (int **) &graph,
            NUM_VERTICES,
            src
        );

#if PERSIST
    while (traversal_result.size) {
        printf("%d\n", traversal_result.front->value);
        pop(&traversal_result);
    }
#endif


    return 0;
}
