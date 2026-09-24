#pragma once

#include "ICache.hpp"
#include <vector>
#include <memory>

template <typename Key, typename Value>
class MultiLevelCache {
private:
    std::vector<std::unique_ptr<ICache<Key, Value>>> levels; // указатель, сам удалит объект, если вектор уничтожится. нельзя копировать, можно только перемещать
    size_t total_hits = 0;

public:
    void addLevel(std::unique_ptr<ICache<Key, Value>> level) {
        levels.push_back(std::move(level));
    }

    bool access(const Key& key) {
        Value dummy{}; // временная переменная для значения
        int hit_level = -1;

        for (size_t i = 0; i < levels.size(); ++i) {
            if (levels[i]->get(key, dummy)) {
                hit_level = static_cast<int>(i);
                break;
            }
        }

        if (hit_level != -1) {
            total_hits++;
            for (int j = 0; j < hit_level; ++j) {
                levels[j]->put(key, dummy);
            }
            return true;
        }

        for (auto& level : levels) {
            level->put(key, Value{});
        }
        return false;
    }

    size_t getTotalHits() const { return total_hits; }
};
