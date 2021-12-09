#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <queue>
#include <algorithm>

#define RUNTIME_DEBUG_ON 0
#define RUNTIME_DEBUG if (RUNTIME_DEBUG_ON) std::cerr

#define RUNTIME_ASSERT_ON 0
#define RUNTIME_ASSERT if (RUNTIME_ASSERT_ON) assert

#define TIME 1
#define TIME_START(Start) \
    uint64_t Start; \
    if (TIME) Start = rdtsc();
    
#define TIME_END_AND_PRINT(Prefix, Start) \
    if (TIME) { \
        uint64_t End = rdtsc() - Start; \
        std::cerr << Prefix << End << std::endl; \
    }
