#include "../include/FaaSAllocator.hpp"

namespace Allocator
{

/*
 * ---------- Compiler Exposed Methods ---------- 
 */
void Init(void)
{
    /*
     * TOP
     * 
     * Build a new pool and set it to Allocator::TheAllocator
     */
    Allocator::TheAllocator = new Allocator::FaaSAllocator();
    return;
}


void AddAllocator(
    uint64_t BumpID,
    uint64_t BlockSize,
    uint64_t PoolSize
)
{
    /*
     * TOP
     *
     * Add bump allocator to Allocator::TheAllocator 
     */
    Allocator::TheAllocator->AddBumpAllocator(
        BumpID,
        BlockSize,
        PoolSize
    );

    return;
}


void Allocate(uint64_t BumpID)
{
    /*
     * TOP
     * 
     * Wrapper to allocate memory in Allocator::TheAllocator 
     */
    Allocator::TheAllocator->AllocateFromBump(BumpID);
    return;
}


void Free(
    uint64_t BumpID,
    void *Pointer
)
{

    /*
     * TOP
     * 
     * Wrapper to allocate memory in Allocator::TheAllocator 
     */
    Allocator::TheAllocator->FreeFromBump(BumpID, Pointer);
    return;
}


/*
 * -------------------- BumpAllocator --------------------
 */

/*
 * ---------- Constructors ----------
 */
BumpAllocator::BumpAllocator(
    uint64_t BumpID,
    uint64_t BlockSize,
    uint64_t PoolSize
) : BumpID(BumpID), BlockSize(BlockSize), PoolSize(PoolSize) 
{
    /*
     * Allocate and initialize first pool
     */
    _addPool();
}


/*
 * ---------- Drivers ----------
 */
void *BumpAllocator::Allocate(void)
{
    /*
     * TOP
     *
     * Find a block that's free in any of the pools and return 
     * the base pointer for the block found
     */

    /*
     * Iterate over all pools
     */
    void *PoolToUse;
    for (auto const &[Pool, FreeList] : Pools)
    {
        /*
         * Use the first pool that has free blocks
         */
        if (FreeList.size())
        {
            PoolToUse = Pool;
            break;
        }
    }


    /*
     * Fetch the next free block ID
     */
    uint64_t FreeBlockID = Pools[PoolToUse].front();
    Pools[PoolToUse].pop();


    /*
     * Calculate the pointer for the block ID
     */
    uint64_t Offset = BlockSize * FreeBlockID;
    void *Pointer = PoolToUse + Offset;
    return Pointer;
}


void BumpAllocator::Free(void *Pointer)
{
    /*
     * TOP
     *
     * Find the pool that @Pointer belongs to and add 
     * its block ID to the free list for that pool
     */
    
    /*
     * Find pool
     */
    auto Iterator = Pools.lower_bound(Pointer);
    if (!(Iterator->first == Pointer)) Iterator--; /* Fetch the actual pool unless it's the base pointer */

    
    /*
     * Calculate the block ID
     */
    uint64_t BlockID = 
        ((uint64_t) Pointer - (uint64_t) (Iterator->first)) / BlockSize;


    /*
     * Add to free list for the pool
     */
    Iterator->second.push(BlockID);


    return;
}


/*
 * ---------- Private Methods ----------
 */
void BumpAllocator::_addPool(void)
{
    /*
     * TOP
     *
     * Wrapper for allocating and initializing a new pool for the allocator
     */

    /*
     * Allocate pool
     */
    void *Pool = malloc(PoolSize);
    RUNTIME_ASSERT(Pool);
    

    /*
     * Record the pool and free list
     */
    Pools[Pool] = std::queue<uint64_t>();
    for (auto ID = 0 ; ID < (PoolSize / BlockSize) ; ID++) Pools[Pool].push(ID);


    return;
}




/*
 * -------------------- FaaSAllocator --------------------
 */

/*
 * ---------- Constructors ----------
 */
FaaSAllocator::FaaSAllocator(void) {}


/*
 * ---------- Drivers ----------
 */
void FaaSAllocator::Init(uint64_t PoolSize)
{
    /*
     * TOP
     *
     * Initialize the first pool and add to list of pools
     */

    /*
     * Allocate
     */
    void *Pool = malloc(PoolSize);
    if (!Pool) 
    {
        RUNTIME_DEBUG << "FaaSAllocator::InitPool: Pool allocation failed!" << std::endl;
        if (RUNTIME_ASSERT_ON) abort();
        return false;
    }

    
    /*
     * Record
     */
    Allocator::CompilerPartitionedPool = Pool;


    /*
     * NOTE --- All bump allocators are directed through the compiler
     * and therefore not handled in this constructor.
     */


    return;
}


/*
 * ---------- Private Methods ----------
 */


}