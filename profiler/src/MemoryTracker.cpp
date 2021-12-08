#include "../include/MemoryTracker.hpp"


/*
 * ---------- Compiler Exposed Functions ----------
 */
__attribute__((constructor)) void Track::InitializeTracker(void)
{
    /*
     * TOP
     * 
     * Construct a new MemoryTracker
     */
    Track::Tracker = new Track::MemoryTracker();
    return;
}


__attribute__((destructor)) void Track::DumpTracker(void)
{
    /*
     * TOP
     * 
     * Dump all information from the global tracker
     */
    Track::Tracker->Dump();
    PROFILER_DEBUG << "Track::DumpTracker: Finished!" << std::endl;
    return;
}


void Track::TrackAllocation(
    void *Pointer, 
    size_t Size
)
{
    /*
     * TOP
     *
     * Track @Pointer with size @Size in @Track::Tracker->Allocations
     * and update state on the working set size
     */

    /*
     * Save allocation
     */
    PROFILER_DEBUG << "Track::TrackAllocation: [Pointer : Size] = [" 
                   << std::hex << Pointer << " : "
                   << std::dec << Size
                   << "]" << std::endl;

    assert(
        Track::Tracker->Allocations.find(Pointer) == Track::Tracker->Allocations.end()
        && "Track::TrackAllocation: @Pointer already has a entry!"
    );

    Track::Tracker->Allocations[Pointer] = Size;


    /*
     * Update working set size state
     */
    uint64_t LastTrackedSize = Track::Tracker->WorkingSetSizesPerTrackingEvent.back();
    uint64_t NewSizeToTrack = LastTrackedSize + Size;
    Track::Tracker->WorkingSetSizesPerTrackingEvent.push_back(NewSizeToTrack);


    return;
}


void Track::TrackDeallocation(void *Pointer)
{
    /*
     * TOP
     *
     * Remove @Pointer from @Track::Tracker->Allocations and 
     * update state on the working set size
     */

    /*
     * Fetch size for @Pointer
     */
    assert(
        Track::Tracker->Allocations.find(Pointer) != Track::Tracker->Allocations.end()
        && "Track::TrackDeallocation: @Pointer doesn't have entry!"
    );

    uint64_t AllocationSize = Track::Tracker->Allocations[Pointer];

    PROFILER_DEBUG << "Track::TrackDeallocation: [Pointer : Size] = [" 
                   << std::hex << Pointer << " : "
                   << std::dec << AllocationSize
                   << "]" << std::endl;


    /*
     * Update working set size state
     */    
    uint64_t LastTrackedSize = Track::Tracker->WorkingSetSizesPerTrackingEvent.back();
    uint64_t NewSizeToTrack = LastTrackedSize - AllocationSize;
    Track::Tracker->WorkingSetSizesPerTrackingEvent.push_back(NewSizeToTrack);


    /*
     * Remove allocation, add it to deallocations
     */
    Track::Tracker->Allocations.erase(Pointer);
    Track::Tracker->Deallocations[Pointer]++;


    return;
}


void Track::TrackLoad(void *Pointer)
{
    /*
     * TOP
     *
     * Track explicit loads, where the load of @Pointer
     * is a necessary load instruction --- and therefore
     * represented in the IR.
     */
    Track::Tracker->ReferencedLoads[Pointer]++;

    PROFILER_DEBUG << "Track::TrackLoad: [Pointer : Refs] = [" 
                   << std::hex << Pointer << " : "
                   << std::dec << (Track::Tracker->ReferencedLoads[Pointer])
                   << "]" << std::endl;

    return;
}


void Track::TrackStore(void *Pointer)
{
    /*
     * TOP
     *
     * Track explicit stores, where the store to @Pointer
     * is a necessary store instruction --- and therefore
     * represented in the IR.
     */
    Track::Tracker->ReferencedStores[Pointer]++;

    PROFILER_DEBUG << "Track::TrackStore: [Pointer : Refs] = [" 
                   << std::hex << Pointer << " : "
                   << std::dec << (Track::Tracker->ReferencedStores[Pointer])
                   << "]" << std::endl;

    return;
}


/*
 * ---------- Constructors ----------
 */
Track::MemoryTracker::MemoryTracker(void) {
    WorkingSetSizesPerTrackingEvent.push_back(0); /* Initialize working set size tracking */
}


/*
 * ---------- Drivers ----------
 */
void Track::MemoryTracker::Dump(void)
{
    /*
     * TOP
     *
     * Dump all state in Track::Tracker
     */
    std::cerr << "=== Track::MemoryTracker::Dump ===" << std::endl;
    

    /*
     * Final allocations
     */
    std::cerr << "--- Allocations ---" << std::endl;
    for (auto const &[Pointer, Size] : Allocations)
        std::cerr << std::hex << Pointer << " : " << std::dec << Size << std::endl;


    /*
     * Final deallocations
     */
    std::cerr << "--- Deallocations ---" << std::endl;
    for (auto const &[Pointer, Num] : Deallocations)
        std::cerr << std::hex << Pointer << " : " << std::dec << Num << std::endl;


    /*
     * Explicit load references
     */
    std::cerr << "--- Load References ---" << std::endl;
    for (auto const &[Pointer, Count] : ReferencedLoads)
    {
        std::cerr << std::hex << Pointer << " : " << std::dec << Count << std::endl;
        if (false
            || Allocations.find(Pointer) != Allocations.end()
            || Deallocations.find(Pointer) != Deallocations.end())
            std::cerr << "\tRefers to an allocation/deallocation" << std::endl;
    }


    /*
     * Explicit store references
     */
    std::cerr << "--- Store References ---" << std::endl;
    for (auto const &[Pointer, Count] : ReferencedLoads)
    {
        std::cerr << std::hex << Pointer << " : " << std::dec << Count << std::endl;
        if (false
            || Allocations.find(Pointer) != Allocations.end()
            || Deallocations.find(Pointer) != Deallocations.end())
            std::cerr << "\tRefers to an allocation/deallocation" << std::endl;
    }


    /*
     * Working set size over time
     */
    std::cerr << "--- Working Set Sizes ---" << std::endl;
    for (auto Size : WorkingSetSizesPerTrackingEvent)
        std::cerr << Size << std::endl;


    return;
}