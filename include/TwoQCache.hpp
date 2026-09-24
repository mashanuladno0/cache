#pragma once

#include "ICache.hpp"
#include <list>
#include <unordered_map>
#include <iostream>

// A1 - новые, Аm - частые, A1out - вытесненные из А1
template <typename Key, typename Value>
class TwoQCache : public ICache<Key, Value> {
private:
    struct Node {
        Key key;
        Value value;
        int which; // 0 = A1, 1 = Am, 2 = A1out
        typename std::list<Key>::iterator pos;
    };

    size_t capacity;
    size_t kin; // размер А1
    size_t hit_count = 0;

    std::list<Key> A1, Am, A1out;
    std::unordered_map<Key, Node> registry;

    void check_a1out_overflow() { //проверка переполнения а1аут
        while (!A1out.empty() && A1out.size() > kin) {
            Key ghost = A1out.front();
            A1out.pop_front();
            registry.erase(ghost);
        }
    }

    void evict_resident() {
        if (!A1.empty() && A1.size() >= kin) {
            Key old = A1.front();
            A1.pop_front();
            registry[old].which = 2;
            A1out.push_back(old);
            registry[old].pos = std::prev(A1out.end());
            check_a1out_overflow();
            return;
        }

        if (!Am.empty()) {
            Key old = Am.back();
            Am.pop_back();
            registry.erase(old);
            return;
        }

        if (!A1.empty()) {
            Key old = A1.front();
            A1.pop_front();
            registry.erase(old);
        }
    }

public:
    explicit TwoQCache(size_t cap, size_t k) : capacity(cap), kin(k > 0 ? k : cap / 4) {
        if (capacity > 0 && kin == 0) kin = 1;
        if (kin > capacity) kin = capacity;
    }

    bool get(const Key& key, Value& out) override {
        auto it = registry.find(key);
        if (it == registry.end()) return false;

        Node& node = it->second;

        if (node.which == 1) {
            hit_count++;
            Am.erase(node.pos);
            Am.push_front(key);
            node.pos = Am.begin();
            out = node.value;
            return true;
        }

        if (node.which == 0) {
            hit_count++;
            A1.erase(node.pos);
            node.which = 1;
            Am.push_front(key);
            node.pos = Am.begin();
            out = node.value;
            return true;
        }

        return false; //не хит
    }

    void put(const Key& key, const Value& value) override {
        if (capacity == 0) return;

        auto it = registry.find(key);

        if (it != registry.end() && it->second.which == 1) {
            it->second.value = value;
            Am.erase(it->second.pos);
            Am.push_front(key);
            it->second.pos = Am.begin();
            return;
        }
        if (it != registry.end() && it->second.which == 0) {
            it->second.value = value;
            A1.erase(it->second.pos);
            it->second.which = 1;
            Am.push_front(key);
            it->second.pos = Am.begin();
            return;
        }

        if (it != registry.end() && it->second.which == 2) {
            it->second.value = value;
            A1out.erase(it->second.pos);
            if (A1.size() + Am.size() >= capacity) {
                evict_resident();
            }
            it->second.which = 1;
            Am.push_front(key);
            it->second.pos = Am.begin();
            return;
        }

        // новый ключ
        if (A1.size() + Am.size() >= capacity) {
            evict_resident();
        }
        A1.push_back(key);
        registry[key] = Node{key, value, 0, std::prev(A1.end())};
    }

    size_t getHits() const override { return hit_count; }

    void print() const override {
        std::cout << "2Q Cache (hits: " << hit_count << ", kin=" << kin << ")\n";
        std::cout << "  A1:    ";
        for (auto& k : A1) {
            std::cout << k << " ";
        }
        std::cout << "\n  Am:    ";
        for (auto& k : Am) {
            std::cout << k << " ";
        }
        std::cout << "\n  A1out: ";
        for (auto& k : A1out) {
            std::cout << k << " ";
        }
        std::cout << "\n";
    }
};
