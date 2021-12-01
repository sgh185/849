#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_map>

#define PROFILER_DEBUG_ON 0
#define PROFILER_DEBUG if (PROFILER_DEBUG_ON) std::cerr

#define PROFILER_ASSERT_ON 0
#define PROFILER_ASSERT if (PROFILER_ASSERT_ON) std::assert

