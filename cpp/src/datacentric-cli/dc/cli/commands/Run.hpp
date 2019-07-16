#pragma once

#include <cli/CLI11.hpp>

struct RunOptions
{
    std::string Source;
    std::string Environment;
    std::string Dataset;
    std::string Key;
    std::string Type;
    std::string Handler;
    std::vector<std::string> Arguments;
};

void SetupRun(CLI::App& app);
void Run(RunOptions const& opt);
