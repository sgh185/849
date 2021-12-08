#pragma once

#include "ProfilerConfigurations.hpp"


/*
 * NOTE
 *
 * This is an odd design for the memory tracker b/c
 * of methods need to be exposed to the compiler easily
 */


namespace Track
{
    class MemoryTracker;

    /*
     * Methods exposed for the compiler
     */
    void InitializeTracker(void);

    void DumpTracker(void);

    void TrackAllocation(
        void *Pointer, 
        size_t Size
    );

    void TrackDeallocation(void *Pointer);

    void TrackLoad(void *Pointer);

    void TrackStore(void *Pointer);


    /*
     * Global tracker
     */
    MemoryTracker *Tracker=nullptr;


    class MemoryTracker
    {

    public:

        /*
         * Constructors
         */
        MemoryTracker(void);


        /*
         * Drivers
         */
        void Dump(void);


        /*
         * Exposed state
         */
        std::unordered_map<void *, uint64_t /* Size of allocation */> Allocations;

        std::unordered_map<void *, uint64_t /* Number of deallocations for a pointer */> Deallocations;

        std::unordered_map<void *, uint64_t> ReferencedStores;

        std::unordered_map<void *, uint64_t> ReferencedLoads;

        std::vector<uint64_t> WorkingSetSizesPerTrackingEvent; /* Records total size of allocations over time */
        
    };


}
