#pragma once

#include <string>
#include <vector>

struct Config {
    int num_levels = 0;
    std::vector<int> capacities;
    std::vector<std::string> algorithms;
};

Config readConfig(const std::string& filename);