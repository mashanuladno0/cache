#include "TestFramework.hpp"
#include "MultiLevelCache.hpp"
#include "LFUCache.hpp"
#include "ARCCache.hpp"

TEST(MultiLevel_SingleLevelBasic) {
    MultiLevelCache<int, int> sys;
    sys.addLevel(std::make_unique<LFUCache<int, int>>(2));

    CHECK_FALSE(sys.access(1));
    CHECK(sys.access(1));
    CHECK_EQ(sys.getTotalHits(), 1);
}

TEST(MultiLevel_TwoLevelsBasic) {
    MultiLevelCache<int, int> sys;
    sys.addLevel(std::make_unique<LFUCache<int, int>>(2));
    sys.addLevel(std::make_unique<LFUCache<int, int>>(4));

    CHECK_FALSE(sys.access(5));
    CHECK(sys.access(5));
    CHECK_EQ(sys.getTotalHits(), 1);
}

TEST(MultiLevel_PromotionFromL2) {
    MultiLevelCache<int, int> sys;
    sys.addLevel(std::make_unique<LFUCache<int, int>>(1));
    sys.addLevel(std::make_unique<LFUCache<int, int>>(4));

    sys.access(1);
    sys.access(2);
    sys.access(3);

    CHECK(sys.access(2));
    CHECK_EQ(sys.getTotalHits(), 1);
}

TEST(MultiLevel_TotalHits) {
    MultiLevelCache<int, int> sys;
    sys.addLevel(std::make_unique<ARCCache<int, int>>(3));

    sys.access(1);
    sys.access(2);
    sys.access(1);
    sys.access(2);
    sys.access(3);
    sys.access(1);
    CHECK_EQ(sys.getTotalHits(), 3);
}
