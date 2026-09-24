#include "TestFramework.hpp"
#include "IdealCache.hpp"
#include <vector>

static size_t ideal_run(const std::vector<int>& trace, size_t cap) {
    IdealCache<int, int> cache(cap, trace);
    size_t hits = 0;
    for (int k : trace) {
        int out = 0;
        if (cache.get(k, out)) hits++;
        else cache.put(k, k);
    }
    return hits;
}

TEST(Ideal_EmptyTraceReturnsMiss) {
    std::vector<int> trace;
    IdealCache<int, int> cache(2, trace);
    int out = 0;
    CHECK_FALSE(cache.get(1, out));
}

TEST(Ideal_PutThenGetReturnsHit) {
    std::vector<int> trace = {1, 1};
    IdealCache<int, int> cache(2, trace);
    int out = 0;
    cache.get(1, out);
    cache.put(1, 100);
    CHECK(cache.get(1, out));
    CHECK_EQ(out, 100);
}

TEST(Ideal_CyclicTrace) {
    CHECK_EQ(ideal_run({1, 2, 3, 1, 2, 3}, 3), 3);
}

TEST(Ideal_SequentialNoReuse) {
    CHECK_EQ(ideal_run({1, 2, 3, 4, 5}, 2), 0);
}

TEST(Ideal_KnowsFuture) {
    CHECK_EQ(ideal_run({1, 2, 3, 2, 1, 3}, 2), 2);
}
