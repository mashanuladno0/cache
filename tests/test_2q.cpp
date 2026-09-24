#include "TestFramework.hpp"
#include "TwoQCache.hpp"

using IntCache = TwoQCache<int, int>;

TEST(TwoQ_EmptyCacheReturnsMiss) {
    IntCache cache(2, 1);
    int out = 0;
    CHECK_FALSE(cache.get(1, out));
}

TEST(TwoQ_PutThenGetReturnsHit) {
    IntCache cache(2, 1);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK_EQ(out, 10);
}

TEST(TwoQ_SecondAccessIsHit) {
    IntCache cache(2, 1);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
    CHECK_EQ(cache.getHits(), 1);
}

TEST(TwoQ_FrequencyProtectsFromEviction) {
    IntCache cache(2, 1);
    cache.put(1, 1);
    cache.put(2, 2);
    int out = 0;
    cache.get(1, out);
    cache.get(1, out);
    cache.put(3, 3);
    CHECK(cache.get(1, out));
    CHECK_FALSE(cache.get(2, out));
}

TEST(TwoQ_CapacityOne) {
    IntCache cache(1, 1);
    cache.put(1, 10);
    int out = 0;
    CHECK(cache.get(1, out));
}
