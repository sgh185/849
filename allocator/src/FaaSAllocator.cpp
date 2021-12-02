#include "../include/FaaSAllocator.hpp"

namespace Allocator
{


/*
 * ---------- Compiler Exposed Methods ---------- 
 */
void Init(uint64_t PoolSize)
{
    /*
     * TOP
     * 
     * Build the compiler directed pool and set it to Allocator::TheAllocator
     */
    Allocator::TheAllocator = new Allocator::FaaSAllocator();
    Allocator::TheAllocator->Init(PoolSize);
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


void *AllocateWithRuntimeInit(
    uint64_t BumpID,
    uint64_t BlockSize
)
{
    /*
     * TOP
     * 
     * Wrapper to allocate memory w/ runtime init in Allocator::TheAllocator 
     */
    void *Allocation = Allocator::TheAllocator->AllocateFromBumpWithRuntimeInit(BumpID, BlockSize);
    return Allocation;
}


void *Allocate(uint64_t BumpID)
{
    /*
     * TOP
     * 
     * Wrapper to allocate memory in Allocator::TheAllocator 
     */
    void *Allocation = Allocator::TheAllocator->AllocateFromBump(BumpID);
    return Allocation;
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
    uint64_t PointerAddr = ((uint64_t) PoolToUse) + Offset;
    return ((void *) PointerAddr);
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
        return;
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


void FaaSAllocator::AddBumpAllocator(
    uint64_t BumpID,
    uint64_t BlockSize,
    uint64_t PoolSize
)
{
    /*
     * TOP
     *
     * Create a new bump allocator and record it
     */
    BumpPools[BumpID] = 
        new BumpAllocator (
            BumpID,
            BlockSize,
            PoolSize
        );

    return;
}

void *FaaSAllocator::AllocateFromBumpWithRuntimeInit(
    uint64_t BumpID,
    uint64_t BlockSize
)
{
    /*
     * TOP
     *
     * Allocate from the bump allocator specified by @BumpID. This
     * method supports the possibility of initializing the allocator
     * during runtime b/c the block size was unknown at compile
     * time. Here, the block size is simply a parameter and the 
     * pool size is the default size. 
     */

    /*
     * Initialize bump allocator if necessary
     */
    if (BumpPools.find(BumpID) == BumpPools.end())
    {
        BumpPools[BumpID] = 
            new BumpAllocator (
                BumpID,
                BlockSize,
                Allocator::DefaultNumPoolEntries * BlockSize
            );
    }


    /*
     * Now allocate
     */
    void *Allocation = AllocateFromBump(BumpID);
    return Allocation;
}


void *FaaSAllocator::AllocateFromBump(
    uint64_t BumpID
)
{
    /*
     * TOP
     *
     * Allocate from the bump allocator specified by @BumpID.
     */

    /*
     * Fetch the bump allocator
     */
    BumpAllocator *BA = BumpPools[BumpID];


    /*
     * Create allocation and return
     */
    void *Allocation = BA->Allocate();
    return Allocation;
}


void FaaSAllocator::FreeFromBump(
    uint64_t BumpID,
    void *Pointer
)
{
    /*
     * TOP
     *
     * Free @Pointer from the bump allocator specified by @BumpID.
     */

    /*
     * Fetch the bump allocator
     */
    BumpAllocator *BA = BumpPools[BumpID];


    /*
     * Create allocation and return
     */
    BA->Free(Pointer);
    return ;
}




/*
 * ---------- Private Methods ----------
 */


}