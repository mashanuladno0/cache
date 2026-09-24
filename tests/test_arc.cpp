#include "TestFramework.hpp"
#include "ARCCache.hpp"
#include <vector>

using IntCache = ARCCache<int, int>;

static size_t simulate(IntCache& cache, const std::vector<int>& keys) {
    size_t hits = 0;
    for (int k : keys) {
        int out = 0;
        if (cache.get(k, out)) hits++;
        else cache.put(k, k * 10);
    }
    return hits;
}

TEST(ARC_EmptyCacheReturnsMiss) {
    IntCache cache(2);
    int out = 0;
    CHECK_FALSE(cache.get(1, out));
}

TEST(ARC_PutThenGetReturnsHit) {
    IntCache cache(2);
    cache.put(1, 100);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK_EQ(out, 100);
}

TEST(ARC_CapacityOne) {
    IntCache cache(1);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
    cache.put(2, 20);
    CHECK(cache.get(2, out));
}

TEST(ARC_RepeatedAccessHits) {
    IntCache cache(2);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK(cache.get(1, out));
    CHECK(cache.get(1, out));
    CHECK_EQ(cache.getHits(), 3);
}

TEST(ARC_SimulateCyclicTrace) {
    IntCache cache(3);
    size_t hits = simulate(cache, {1, 2, 3, 1, 2, 3});
    CHECK_EQ(hits, 3);
}

TEST(ARC_SimulateAlternatingTrace) {
    IntCache cache(2);
    size_t hits = simulate(cache, {1, 2, 1, 2, 1, 2});
    CHECK_EQ(hits, 4);
}
