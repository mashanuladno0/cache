#include "Config.hpp"
#include "MultiLevelCache.hpp"
#include "CacheFactory.hpp"
#include "IdealCache.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

// Вся логика — в шаблонной функции.
// Конкретный тип выбирается в main() (см. ниже).
template <typename Key, typename Value>
int run(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <config> <input>\n";
        return 1;
    }

    Config cfg = readConfig(argv[1]);
    if (cfg.num_levels <= 0) return 1;

    // Читаем весь trace (нужен для IDEAL)
    std::vector<Key> trace;
    std::ifstream data(argv[2]);
    if (!data) {
        std::cerr << "Error opening input file\n";
        return 1;
    }
    Key k;
    while (data >> k) trace.push_back(k);

    // 1. Строим реальную систему, пропуская IDEAL
    MultiLevelCache<Key, Value> system;
    bool ideal_requested = false;
    int total_capacity = 0;

    for (int i = 0; i < cfg.num_levels; ++i) {
        total_capacity += cfg.capacities[i];
        if (cfg.algorithms[i] == "IDEAL") {
            ideal_requested = true;
            continue;
        }
        auto cache = createCache<Key, Value>(
            cfg.algorithms[i],
            static_cast<size_t>(cfg.capacities[i])
        );
        if (!cache) return 1;
        system.addLevel(std::move(cache));
    }

    // 2. Прогон через реальную систему
    for (const Key& key : trace) {
        system.access(key);
    }

    // 3. Печатаем число хитов реальной системы (это то, что просит задание)
    std::cout << system.getTotalHits() << "\n";

    // 4. Если в конфиге был IDEAL — считаем его отдельно и печатаем для сравнения
    if (ideal_requested) {
        IdealCache<Key, Value> ideal(static_cast<size_t>(total_capacity), trace);
        for (const Key& key : trace) {
            Value dummy{};
            if (!ideal.get(key, dummy)) {
                ideal.put(key, dummy);
            }
        }
        std::cout << "Ideal: " << ideal.getHits() << "\n";
    }

    return 0;
}

// Точка входа. Здесь выбираются конкретные типы Key и Value.
// В C++ main() не может быть шаблоном, поэтому конкретный тип нужен
// ровно один раз — здесь. Чтобы сменить тип, поменяйте эти две строки.
int main(int argc, char** argv) {
    using Key   = int64_t;
    using Value = int64_t;
    return run<Key, Value>(argc, argv);
}
