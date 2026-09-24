#pragma once

#include "ICache.hpp"
#include <list>
#include <unordered_map>
#include <algorithm>
#include <iostream>

// LIR (горячие, всегда в кеше) и HIR (холодные, могут быть вытеснены, но остаются без значений в стеке S)
// S - стек всех блоков, Q - кандидаты на вытеснение
// 
template <typename Key, typename Value>
class LIRSCache : public ICache<Key, Value> {
private:
    struct Node {
        Key key;
        Value value;
        bool is_lir      = false; // lir or hir
        bool is_resident = false; // есть ли значение в кеше
        bool in_queue    = false; // лежит ли в Q
        typename std::list<Key>::iterator stack_pos; // позиция в S
        typename std::list<Key>::iterator queue_pos; // в Q
    };

    size_t capacity;
    size_t stack_limit;
    size_t hit_count = 0;

    std::list<Key> stack_S;
    std::list<Key> queue_Q;
    std::unordered_map<Key, Node> registry;

    void enqueue(Node& n) { // добавить узел в Q, если его там нет
        if (!n.in_queue) {
            queue_Q.push_back(n.key);
            n.queue_pos = std::prev(queue_Q.end());
            n.in_queue = true;
        }
    }

    void dequeue(Node& n) { // убрать узел из Q
        if (n.in_queue) {
            queue_Q.erase(n.queue_pos);
            n.in_queue = false;
        }
    }

    void promote(Node& n) { // сделать узел lir
        n.is_lir = true;
        dequeue(n);
    }

    void demote(Node& n) { // сделать узел hir
        n.is_lir = false;
        if (n.is_resident) enqueue(n);
    }

    void move_to_front_stack(const Key& key, Node& n) { // в начало s
        stack_S.erase(n.stack_pos);
        stack_S.push_front(key);
        n.stack_pos = stack_S.begin();
    }

    // переполнение s
    void trim_stack() {
        while (stack_S.size() > stack_limit) {
            Key victim = stack_S.back();
            auto it = registry.find(victim);
            if (it != registry.end() && it->second.is_resident) {
                break; // резидентных не трогаем
            }
            stack_S.pop_back();
            if (it != registry.end()) registry.erase(it);
        }
    }

    // повышаем hir до lir, понизив самый старый lir
    void try_promote(const Key& key, Node& n) {
        if (n.is_lir) return;
        for (auto rit = stack_S.rbegin(); rit != stack_S.rend(); ++rit) {
            if (*rit == key) continue;
            auto vit = registry.find(*rit);
            if (vit != registry.end() && vit->second.is_lir) {
                demote(vit->second);
                promote(n);
                return;
            }
        }
    }

public:
    explicit LIRSCache(size_t cap)
        : capacity(cap),
          stack_limit(cap + std::max<size_t>(1, cap / 2)) {}

    bool get(const Key& key, Value& out) override {
        auto it = registry.find(key);
        if (it == registry.end()) return false;

        Node& n = it->second;
        if (!n.is_resident) return false;

        hit_count++;
        out = n.value;
        move_to_front_stack(key, n);
        try_promote(key, n);
        return true;
    }

    void put(const Key& key, const Value& value) override {
        if (capacity == 0) return;

        auto it = registry.find(key);

        if (it != registry.end()) {
            Node& n = it->second;
            n.value = value;

            if (!n.is_resident) {
                // освобождаем место, если нужно
                if (queue_Q.size() >= capacity) {
                    Key old = queue_Q.front();
                    queue_Q.pop_front();
                    auto oit = registry.find(old);
                    if (oit != registry.end()) {
                        oit->second.is_resident = false;
                        oit->second.in_queue = false;
                    }
                }
                n.is_resident = true;
                enqueue(n);
            }

            move_to_front_stack(key, n);
            try_promote(key, n);
            trim_stack();
            return;
        }

        // новый ключ
        if (queue_Q.size() >= capacity) {
            Key old = queue_Q.front();
            queue_Q.pop_front();
            auto oit = registry.find(old);
            if (oit != registry.end()) {
                oit->second.is_resident = false;
                oit->second.in_queue = false;
            }
        }

        Node n;
        n.key = key;
        n.value = value;
        n.is_lir = false;
        n.is_resident = true;
        n.in_queue = true;
        stack_S.push_front(key);
        n.stack_pos = stack_S.begin();
        queue_Q.push_back(key);
        n.queue_pos = std::prev(queue_Q.end());
        registry[key] = n;

        trim_stack();
    }

    size_t getHits() const override { return hit_count; }

    void print() const override {
        std::cout << "LIRS Cache (hits: " << hit_count << ")\n";
        std::cout << "  S: "; for (auto& k : stack_S) std::cout << k << " ";
        std::cout << "\n  Q: "; for (auto& k : queue_Q) std::cout << k << " ";
        std::cout << "\n";
    }
};