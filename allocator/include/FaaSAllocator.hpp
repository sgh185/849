#pragma once

#include "RuntimeConfigurations.hpp"


namespace Allocator
{

class FaaSAllocator;

uint64_t DefaultNumPoolEntries=32;


/*
 * Global pool
 */
FaaSAllocator *TheAllocator;

void *CompilerPartitionedPool;


/*
 * Compiler exposed methods for allocator 
 */
void Constructor(void);

void Init(uint64_t PoolSize);

void AddAllocator(
    uint64_t BumpIDBlockSize,
    uint64_t PoolSize
);

void *AllocateFromCompilerDirectedPool(uint64_t Offset);

void *Allocate(uint64_t BumpIDBlockSize);

void *AllocateWithRuntimeInit(uint64_t BumpIDBlockSize);

void Free(void *Pointer);


/*
 * Bump allocator state
 */
class BumpAllocator
{

public:

    /*
     * Constructors
     */
    BumpAllocator(
        uint64_t BumpIDBlockSize,
        uint64_t NumPoolEntries
    );


    /*
     * Drivers
     */
    void *Allocate(void);

    void Free(void *Pointer);


    /*
     * Public state
     */
    uint64_t BumpIDBlockSize;
    uint64_t PoolSize;
    
    std::map<
        void *, /* Pool */
        std::queue<uint64_t> /* Block IDs */
    > Pools;


private:

    /*
     * Private methods
     */
    void *_addPool(void);

};


/*
 * Allocator implementation
 */
class FaaSAllocator
{

public:

    /*
     * Constructors
     */
    FaaSAllocator(void);


    /*
     * Drivers
     */
    void Init(uint64_t PoolSize);

    void AddBumpAllocator(
        uint64_t BumpIDBlockSize,
        uint64_t PoolSize
    );

    void *AllocateFromBump(uint64_t BumpIDBlockSize);

    void *AllocateFromBumpWithRuntimeInit(uint64_t BumpIDBlockSize);

    void FreeFromBump(
        uint64_t BumpIDBlockSize,
        void *Pointer
    );


private:

    /*
     * Private state
     */
    std::unordered_map<
        uint64_t /* ID/Block Size */, 
        BumpAllocator * /* Bump pools */
    > BumpPools;

};

}