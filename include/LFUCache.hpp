#pragma once

#include "ICache.hpp"
#include <list>
#include <unordered_map>
#include <iostream>


template <typename Key, typename Value>
class LFUCache : public ICache<Key, Value> {
private:
    struct Node {
        Key key;
        Value value;
        int freq;
        typename std::list<Key>::iterator pos; // итератор, pos хранит место в списке, где лежит узел
    };

    size_t capacity;
    size_t hit_count = 0;
    int min_freq = 0;
    std::unordered_map<Key, Node> registry; // ключ -> узел
    std::unordered_map<int, std::list<Key>> freq_lists; // частота -> список ключей с такой частотой

    // перемещаем узел в список с частотой +1
    void touch(Node& node) {
        int old_freq = node.freq;

        freq_lists[old_freq].erase(node.pos);
        if (freq_lists[old_freq].empty()) {
            freq_lists.erase(old_freq);
            if (min_freq == old_freq) min_freq = old_freq + 1;
        }

        node.freq = old_freq + 1;
        freq_lists[node.freq].push_front(node.key);
        node.pos = freq_lists[node.freq].begin();
    }

public:
    explicit LFUCache(size_t cap) : capacity(cap) {} // explicit - явный вызов, передаем размер кэша и инициализируем  

    bool get(const Key& key, Value& out) override {
        auto it = registry.find(key); // auto - самоопределение типа 
        if (it == registry.end()) return false;

        hit_count++;
        out = it -> second.value;
        touch(it -> second); // увелич частоту
        return true;
    }

    void put(const Key& key, const Value& value) override {
        if (capacity == 0) return;

        auto it = registry.find(key);
        if (it != registry.end()) {
            it -> second.value = value;
            touch(it -> second);
            return;
        }

        // кэш полон, вытесняем самый старый среди самых редких
        if (registry.size() >= capacity) {
            auto& lst = freq_lists[min_freq];
            Key victim = lst.back();
            lst.pop_back();
            if (lst.empty()) freq_lists.erase(min_freq);
            registry.erase(victim);
        }

        freq_lists[1].push_front(key);
        registry[key] = Node{key, value, 1, freq_lists[1].begin()};
        min_freq = 1;
    }

    size_t getHits() const override { return hit_count; }

    void print() const override {
        std::cout << "LFU Cache (hits: " << hit_count << ")\n";
        for (const auto& [freq, lst] : freq_lists) {
            std::cout << "  freq " << freq << ": ";
            for (const auto& k : lst) std::cout << k << " ";
            std::cout << "\n";
        }
    }
};