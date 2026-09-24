#include "TestFramework.hpp"
#include "Config.hpp"
#include <fstream>
#include <cstdio>

static void write_file(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

TEST(Config_ReadValid) {
    write_file("_test_cfg.txt", "2\n4 LFU\n8 ARC\n");
    Config cfg = readConfig("_test_cfg.txt");
    CHECK_EQ(cfg.num_levels, 2);
    CHECK_EQ((int)cfg.capacities.size(), 2);
    CHECK_EQ(cfg.capacities[0], 4);
    CHECK_EQ(cfg.capacities[1], 8);
    CHECK_EQ(cfg.algorithms[0], "LFU");
    CHECK_EQ(cfg.algorithms[1], "ARC");
    std::remove("_test_cfg.txt");
}

TEST(Config_InvalidNumLevels) {
    write_file("_test_cfg2.txt", "0\n");
    Config cfg = readConfig("_test_cfg2.txt");
    CHECK_EQ(cfg.num_levels, 0);
    std::remove("_test_cfg2.txt");
}

TEST(Config_MissingFile) {
    Config cfg = readConfig("_does_not_exist_12345.txt");
    CHECK_EQ(cfg.num_levels, 0);
}
