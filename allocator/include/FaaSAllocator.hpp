#pragma once

#include "RuntimeConfigurations.hpp"


namespace Allocator
{

class FaaSAllocator;

/*
 * Global pool
 */
FaaSAllocator *TheAllocator;

void *CompilerPartitionedPool;


/*
 * Compiler exposed methods for allocator 
 */
void Init(void);

void AddAllocator(
    uint64_t BumpID,
    uint64_t BlockSize,
    uint64_t PoolSize
);

void Allocate(uint64_t BumpID);

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
        uint64_t BumpID,
        uint64_t BlockSize,
        uint64_t PoolSize
    );


    /*
     * Drivers
     */
    void *Allocate(void);

    void Free(void *Pointer);


    /*
     * Public state
     */
    uint64_t BumpID;

    uint64_t BlockSize;

    uint64_t PoolSize;
    
    std::map<
        void *, /* Pool */
        std::queue<uint64_t> /* Block IDs */
    > Pools;


private:

    /*
     * Private methods
     */
    void _addPool(void);

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
    void Init();

    void AddBumpAllocator(
        uint64_t BumpID,
        uint64_t BlockSize,
        uint64_t PoolSize
    );

    void *AllocateFromBump(uint64_t BumpID);

    void FreeFromBump(
        uint64_t BumpID,
        void *Pointer
    );


private:

    /*
     * Private state
     */
    std::unordered_map<
        uint64_t /* ID */, 
        BumpAllocator * /* Bump pools */
    > BumpPools;

};

}