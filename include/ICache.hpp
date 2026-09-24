#pragma once

#include <cstddef>

template <typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default; // деструктор

    virtual bool get(const Key& key, Value& out) = 0;
    virtual void put(const Key& key, const Value& value) = 0;
    virtual size_t getHits() const = 0;
    virtual void print() const = 0;
};
