#include "TestFramework.hpp"
#include "LIRSCache.hpp"
#include <vector>

using IntCache = LIRSCache<int, int>;

static size_t simulate(IntCache& cache, const std::vector<int>& keys) {
    size_t hits = 0;
    for (int k : keys) {
        int out = 0;
        if (cache.get(k, out)) hits++;
        else cache.put(k, k * 10);
    }
    return hits;
}

TEST(LIRS_EmptyCacheReturnsMiss) {
    IntCache cache(2);
    int out = 0;
    CHECK_FALSE(cache.get(1, out));
}

TEST(LIRS_PutThenGetReturnsHit) {
    IntCache cache(2);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK_EQ(out, 10);
}

TEST(LIRS_CapacityOne) {
    IntCache cache(1);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
}

TEST(LIRS_RepeatedAccessHits) {
    IntCache cache(3);
    cache.put(1, 1);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK(cache.get(1, out));
    CHECK_EQ(cache.getHits(), 2);
}

TEST(LIRS_SimulateCyclicTrace) {
    IntCache cache(3);
    size_t hits = simulate(cache, {1, 2, 3, 1, 2, 3});
    CHECK_EQ(hits, 3);
}
