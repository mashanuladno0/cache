#include "TestFramework.hpp"
#include "LFUCache.hpp"
#include <string>
#include <vector>

using IntCache = LFUCache<int, int>;

static size_t simulate(IntCache& cache, const std::vector<int>& keys) {
    size_t hits = 0;
    for (int k : keys) {
        int out = 0;
        if (cache.get(k, out)) hits++;
        else cache.put(k, k * 10);
    }
    return hits;
}

TEST(LFU_EmptyCacheReturnsMiss) {
    IntCache cache(2);
    int out = 0;
    CHECK_FALSE(cache.get(1, out));
    CHECK_FALSE(cache.get(42, out));
}

TEST(LFU_PutThenGetReturnsHit) {
    IntCache cache(2);
    cache.put(1, 100);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK_EQ(out, 100);
}

TEST(LFU_SecondGetIsHit) {
    IntCache cache(2);
    cache.put(7, 70);
    int out = 0;
    CHECK(cache.get(7, out));
    CHECK(cache.get(7, out));
    CHECK_EQ(cache.getHits(), 2);
}

TEST(LFU_HitsCounter) {
    IntCache cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    int out = 0;
    cache.get(1, out);
    cache.get(2, out);
    cache.get(3, out);
    CHECK_EQ(cache.getHits(), 3);
}

TEST(LFU_EvictsLeastFrequent) {
    IntCache cache(2);
    cache.put(1, 10);
    cache.put(2, 20);
    int out = 0;
    cache.get(1, out);
    cache.put(3, 30);

    CHECK_FALSE(cache.get(2, out));
    CHECK(cache.get(1, out));
    CHECK(cache.get(3, out));
}

TEST(LFU_CapacityOne) {
    IntCache cache(1);
    cache.put(1, 10);
    cache.put(2, 20);
    int out = 0;
    CHECK_FALSE(cache.get(1, out));
    CHECK(cache.get(2, out));
}

TEST(LFU_UpdateExistingKey) {
    IntCache cache(2);
    cache.put(1, 10);
    cache.put(1, 20);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK_EQ(out, 20);
    CHECK_EQ(cache.getHits(), 1);
}

TEST(LFU_SimulateTrace) {
    IntCache cache(2);
    size_t hits = simulate(cache, {1, 2, 1, 3, 1, 2, 1});
    CHECK_EQ(hits, 3);
}

TEST(LFU_WorksWithStrings) {
    LFUCache<std::string, int> cache(2);
    cache.put("apple", 1);
    cache.put("banana", 2);
    int out = 0;
    CHECK(cache.get("apple", out));
    CHECK_EQ(out, 1);
    CHECK(cache.get("banana", out));
    CHECK_EQ(out, 2);
    CHECK_FALSE(cache.get("cherry", out));
}
