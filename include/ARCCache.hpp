#pragma once

#include "ICache.hpp"
#include <list>
#include <unordered_map>
#include <algorithm> // max и min для p
#include <iostream>

// T1 - ключи, которые запрошены недавно
// T2 - ключи, которые запросили повторно
// В1 - ключи, вытесненные из Т1 (значений нет, помним, что ключ был)
// В2 - ключи, вытесненные из Т2

template <typename Key, typename Value>
class ARCCache : public ICache<Key, Value> {
private:
    struct Node {
        Key key;
        Value value;
        bool has_value; // true = в кеше, false = ключа нет, но мы его помним
        int which;      // в каком списке узел 0=T1, 1=T2, 2=B1, 3=B2
        typename std::list<Key>::iterator pos;
    };

    size_t c; //capacity
    size_t p;

    std::list<Key> T1, B1, T2, B2;
    std::unordered_map<Key, Node> registry;
    size_t hit_count = 0;

    void replace(bool in_B2) {
        if (!T1.empty() &&
            ((T1.size() > p) || (in_B2 && T1.size() == p))) {
            Key old_key = T1.back();
            T1.pop_back();
            B1.push_front(old_key);
            Node& n = registry[old_key];
            n.has_value = false;
            n.which = 2;
            n.pos = B1.begin();
        } else { // если T1 не переполнен, вытесняем из T2 в В2
            if (T2.empty()) return;
            Key old_key = T2.back();
            T2.pop_back();
            B2.push_front(old_key);
            Node& n = registry[old_key];
            n.has_value = false;
            n.which = 3;
            n.pos = B2.begin();
        }
    }

    void move_to_T2(const Key& key, Node& node) {
        switch (node.which) {
            case 0: T1.erase(node.pos); break;
            case 1: T2.erase(node.pos); break;
            case 2: B1.erase(node.pos); break;
            case 3: B2.erase(node.pos); break;
        }
        T2.push_front(key);
        node.which = 1;
        node.pos = T2.begin();
    }

public:
    explicit ARCCache(size_t capacity) : c(capacity), p(0) {}

    bool get(const Key& key, Value& out) override {
        auto it = registry.find(key);
        if (it == registry.end() || !it->second.has_value) return false;

        hit_count++;
        out = it->second.value;
        move_to_T2(key, it->second);
        return true;
    }

    void put(const Key& key, const Value& value) override {
        if (c == 0) return;

        auto it = registry.find(key);

        // уже в кеше
        if (it != registry.end() && it->second.has_value) {
            it->second.value = value;
            move_to_T2(key, it->second);
            return;
        }

        // B1/B2
        if (it != registry.end() && !it->second.has_value) {
            Node& node = it->second;
            node.value = value;
            node.has_value = true;

            if (node.which == 2) {
                size_t b1 = std::max<size_t>(1, B1.size());
                size_t b2 = std::max<size_t>(1, B2.size());
                p = std::min(c, p + std::max<size_t>(1, b2 / b1));
                replace(false);
            } else {
                size_t b1 = std::max<size_t>(1, B1.size());
                size_t b2 = std::max<size_t>(1, B2.size());
                size_t delta = std::max<size_t>(1, b1 / b2);
                p = (p > delta) ? p - delta : 0;
                replace(true);
            }
            move_to_T2(key, node);
            return;
        }

        // новый ключ
        if (T1.size() + B1.size() == c) {
            if (T1.size() < c) {
                if (!B1.empty()) {
                    registry.erase(B1.back());
                    B1.pop_back();
                }
                replace(false);
            } else {
                if (!T1.empty()) {
                    registry.erase(T1.back());
                    T1.pop_back();
                }
            }
        } else if (T1.size() + B1.size() + T2.size() + B2.size() >= c) {
            if (T1.size() + B1.size() + T2.size() + B2.size() == 2 * c) {
                if (!B2.empty()) {
                    registry.erase(B2.back());
                    B2.pop_back();
                }
            }
            replace(false);
        }

        T1.push_front(key);
        registry[key] = Node{key, value, true, 0, T1.begin()};
    }

    size_t getHits() const override { return hit_count; }

    void print() const override {
        std::cout << "ARC Cache (hits: " << hit_count << ", p=" << p << ")\n";
        std::cout << "  T1: "; for (auto& k : T1) std::cout << k << " ";
        std::cout << "\n  T2: "; for (auto& k : T2) std::cout << k << " ";
        std::cout << "\n  B1: "; for (auto& k : B1) std::cout << k << " ";
        std::cout << "\n  B2: "; for (auto& k : B2) std::cout << k << " ";
        std::cout << "\n";
    }
};