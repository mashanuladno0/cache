#pragma once

#include "ICache.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>

template <typename Key, typename Value>
class IdealCache : public ICache<Key, Value> {
private:
    size_t capacity;
    size_t hit_count = 0;
    size_t current_step = static_cast<size_t>(-1); // после первого ++ станет 0

    std::list<std::pair<Key, Value>> cache;
    const std::vector<Key>& trace;

    int findNextUse(const Key& key) const { // ищет, когда данный key встретится в trace после текущего шага
        for (size_t i = current_step + 1; i < trace.size(); ++i) {
            if (trace[i] == key) return static_cast<int>(i);
        }
        return -1;
    }

public:
    IdealCache(size_t cap, const std::vector<Key>& t)
        : capacity(cap), trace(t) {}

    bool get(const Key& key, Value& out) override {
        if (capacity == 0 || trace.empty()) return false;
        current_step++;

        for (auto& [k, v] : cache) {
            if (k == key) {
                hit_count++;
                out = v;
                return true;
            }
        }
        return false;
    }

    void put(const Key& key, const Value& value) override {
        if (capacity == 0) return;

        for (auto& [k, v] : cache) {
            if (k == key) { v = value; return; }
        }

        if (cache.size() < capacity) {
            cache.push_back({key, value});
            return;
        }

        auto victim_it = cache.begin();
        int max_next = findNextUse(victim_it->first);
        if (max_next != -1) {
            for (auto it = std::next(cache.begin()); it != cache.end(); ++it) {
                int next = findNextUse(it->first);
                if (next == -1) { victim_it = it; break; }
                if (next > max_next) { max_next = next; victim_it = it; }
            }
        }
        cache.erase(victim_it);
        cache.push_back({key, value});
    }

    size_t getHits() const override { return hit_count; }

    void print() const override {
        std::cout << "Ideal Cache (hits: " << hit_count << "): ";
        for (auto& [k, v] : cache) std::cout << k << " ";
        std::cout << "\n";
    }
};
