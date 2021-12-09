#include "../include/FaaSAllocator.hpp"

namespace Allocator
{

static inline uint64_t __attribute__((always_inline))
rdtsc (void)
{
    uint32_t lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return lo | ((uint64_t)(hi) << 32);
}


/*
 * ---------- Compiler Exposed Methods ---------- 
 */
__attribute__((used))
void Constructor(void) { return; }


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
    uint64_t BumpIDBlockSize,
    uint64_t PoolSize
)
{
    /*
     * TOP
     *
     * Add bump allocator to Allocator::TheAllocator 
     */
    RUNTIME_DEBUG << "AddAllocator: [BumpIDBlockSize : PoolSize]: ["
                  << BumpIDBlockSize << " : " << PoolSize << "]" << std::endl;
    Allocator::TheAllocator->AddBumpAllocator(
        BumpIDBlockSize,
        PoolSize
    );


    return;
}


void *AllocateFromCompilerDirectedPool(uint64_t Offset)
{
    /*
     * TOP
     *
     * Return a pointer that is offset @Offset into Allocator::CompilerPartitionedPool
     */
    uint64_t Address = ((uint64_t) Allocator::CompilerPartitionedPool) + Offset;
    RUNTIME_DEBUG << "AllocateFromCompilerDirectedPool: [Address : Offset]: ["
                  << Address << " : " << Offset << "]" << std::endl;
    return ((void *) Address);
}


void *AllocateWithRuntimeInit(uint64_t BumpIDBlockSize)
{
    /*
     * TOP
     * 
     * Wrapper to allocate memory w/ runtime init in Allocator::TheAllocator 
     */
    void *Allocation = Allocator::TheAllocator->AllocateFromBumpWithRuntimeInit(BumpIDBlockSize);
    RUNTIME_DEBUG << "AllocateWithRuntimeInit: " << BumpIDBlockSize << std::hex << ", " << Allocation << std::dec << std::endl;
    return Allocation;
}


__attribute__((noinline, optnone))
void *Allocate(uint64_t BumpIDBlockSize)
{
    /*
     * TOP
     * 
     * Wrapper to allocate memory in Allocator::TheAllocator 
     */
    void *Allocation = Allocator::TheAllocator->AllocateFromBump(BumpIDBlockSize);
    RUNTIME_DEBUG  << "Allocate: [BumpIDBlockSize : Allocation]: ["
                   << BumpIDBlockSize << " : " << std::hex << Allocation << std::dec << "]" << std::endl;
    return Allocation;
}


void Free(
    uint64_t BumpIDBlockSize,
    void *Pointer
)
{
    /*
     * TOP
     * 
     * Wrapper to allocate memory in Allocator::TheAllocator 
     */
    Allocator::TheAllocator->FreeFromBump(BumpIDBlockSize, Pointer);
    return;
}


/*
 * -------------------- BumpAllocator --------------------
 */

/*
 * ---------- Constructors ----------
 */
BumpAllocator::BumpAllocator(
    uint64_t BumpIDBlockSize,
    uint64_t PoolSize
) : BumpIDBlockSize(BumpIDBlockSize), PoolSize(PoolSize) 
{
    /*
     * Allocate and initialize first pool
     */
    _addPool();
    return;
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
    RUNTIME_DEBUG << "Here" << std::endl;
    void *PoolToUse;
    for (auto const &[Pool, FreeList] : Pools)
    {
        /*
         * Use the first pool that has free blocks
         */
        if (FreeList.size())
        {
            PoolToUse = Pool;
            RUNTIME_DEBUG << "BumpAllocator::Allocate: Found pool! : " << std::hex << PoolToUse << std::dec << std::endl; 
            break;
        }
    }

    /*
     * Otherwise, add another pool if not found
     */
    if (!PoolToUse) PoolToUse = _addPool();


    /*
     * Fetch the next free block ID
     */
    uint64_t FreeBlockID = Pools[PoolToUse].front();
    RUNTIME_DEBUG  << "BumpAllocator::Allocate: FreeBlockID = " << FreeBlockID << std::endl;
    Pools[PoolToUse].pop();


    /*
     * Calculate the pointer for the block ID
     */
    uint64_t Offset = BumpIDBlockSize * FreeBlockID;
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
        ((uint64_t) Pointer - (uint64_t) (Iterator->first)) / BumpIDBlockSize;


    /*
     * Add to free list for the pool
     */
    Iterator->second.push(BlockID);


    return;
}


/*
 * ---------- Private Methods ----------
 */
void *BumpAllocator::_addPool(void)
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
    for (auto ID = 0 ; ID < (PoolSize / BumpIDBlockSize) ; ID++) {
        RUNTIME_DEBUG  << "BumpAllocator::_addPool: Next BlockID pushed: " << ID << std::endl;
        Pools[Pool].push(ID);
    }


    return Pool;
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
        RUNTIME_DEBUG  << "FaaSAllocator::InitPool: Pool allocation failed!" << std::endl;
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
    uint64_t BumpIDBlockSize,
    uint64_t NumPoolEntries
)
{
    /*
     * TOP
     *
     * Create a new bump allocator and record it
     * 
     * NOTE -- The BumpID *is the same as* the block size
     * The bump allocators are separated by their block sizes
     */
    BumpPools[BumpIDBlockSize] = 
        new BumpAllocator (
            BumpIDBlockSize,
            NumPoolEntries * BumpIDBlockSize
        );


    return;
}

void *FaaSAllocator::AllocateFromBumpWithRuntimeInit(uint64_t BumpIDBlockSize)
{
    /*
     * TOP
     *
     * Allocate from the bump allocator specified by @BumpIDBlockSize. 
     * This method supports the possibility of initializing the allocator
     * during runtime b/c the block size was unknown at compile
     * time. Here, the block size is simply a parameter and the 
     * pool size is the default size. 
     */

    /*
     * Initialize bump allocator if necessary
     */
    if (BumpPools.find(BumpIDBlockSize) == BumpPools.end())
    {
        BumpPools[BumpIDBlockSize] = 
            new BumpAllocator(
                BumpIDBlockSize,
                Allocator::DefaultNumPoolEntries * BumpIDBlockSize
            );
    }


    /*
     * Now allocate
     */
    void *Allocation = AllocateFromBump(BumpIDBlockSize);
    return Allocation;
}


void *FaaSAllocator::AllocateFromBump(uint64_t BumpIDBlockSize)
{
    /*
     * TOP
     *
     * Allocate from the bump allocator specified by @BumpIDBlockSize.
     */

    /*
     * Fetch the bump allocator
     */
    BumpAllocator *BA = BumpPools[BumpIDBlockSize];
    RUNTIME_DEBUG << "FaaSAllocator::AllocateFromBump: BA = " << std::hex << BA << std::dec << std::endl;


    /*
     * Create allocation and return
     */
    void *Allocation = BA->Allocate();
    return Allocation;
}


void FaaSAllocator::FreeFromBump(
    uint64_t BumpIDBlockSize,
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
    BumpAllocator *BA = BumpPools[BumpIDBlockSize];


    /*
     * Create allocation and return
     */
    BA->Free(Pointer);
    return ;
}


}