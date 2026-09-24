#include "Config.hpp"
#include <fstream>
#include <iostream>

Config readConfig(const std::string& filename) {
    Config cfg;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening config: " << filename << "\n";
        return cfg;
    }

    file >> cfg.num_levels;
    if (cfg.num_levels <= 0) {
        std::cerr << "Invalid number of levels\n";
        return cfg;
    }

    for (int i = 0; i < cfg.num_levels; ++i) {
        int cap;
        std::string algo;
        file >> cap >> algo;
        if (!file) {
            std::cerr << "Error reading level " << i + 1 << "\n";
            return cfg;
        }
        cfg.capacities.push_back(cap);
        cfg.algorithms.push_back(algo);
    }
    return cfg;
}