#pragma once

#include "ICache.hpp"
#include "LFUCache.hpp"
#include "ARCCache.hpp"
#include "TwoQCache.hpp"
#include "LIRSCache.hpp"
#include <memory>
#include <string>
#include <iostream>

// Фабрика: по имени алгоритма создаёт соответствующий кеш.
// IDEAL сюда НЕ входит — он создаётся отдельно, потому что ему нужен trace.
template <typename Key, typename Value>
std::unique_ptr<ICache<Key, Value>>
createCache(const std::string& algo, size_t capacity) {
    if (algo == "LFU")  return std::make_unique<LFUCache<Key, Value>>(capacity);
    if (algo == "ARC")  return std::make_unique<ARCCache<Key, Value>>(capacity);
    if (algo == "2Q")   return std::make_unique<TwoQCache<Key, Value>>(capacity, capacity / 4);
    if (algo == "LIRS") return std::make_unique<LIRSCache<Key, Value>>(capacity);
    std::cerr << "Unknown algorithm: " << algo << "\n";
    return nullptr;
}