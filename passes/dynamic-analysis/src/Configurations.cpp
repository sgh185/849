#include "../include/Configurations.hpp"

cl::opt<bool> ExitingOnInit(
    "exit-on-init",
    cl::init(false), 
    cl::desc("Exit the transformation")
);

cl::opt<bool> Debug(
    "pass-debug",
    cl::init(false), 
    cl::desc("Debugging and print-outs")
);

const std::string PassCommandLineOption = "dynamic-analysis";

const std::string PassDescription = "849 -- Dynamic memory analysis pass";

const std::string PassName = "849MemoryAnalysis";