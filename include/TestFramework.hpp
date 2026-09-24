#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>

// Минимальный фреймворк для юнит-тестов.
// Тесты автоматически регистрируются и запускаются все сразу.

namespace test {

struct Registry {
    struct Test {
        std::string name;
        std::function<void()> func;
    };

    std::vector<Test> tests;
    int passed = 0;
    int failed = 0;

    static Registry& get() {
        static Registry r;
        return r;
    }
};

struct Register {
    Register(const std::string& name, std::function<void()> func) {
        Registry::get().tests.push_back({name, func});
    }
};

inline int run_all() {
    auto& r = Registry::get();
    std::cout << "Running " << r.tests.size() << " tests...\n\n";
    for (auto& t : r.tests) {
        int before = r.failed;
        std::cout << "[ RUN      ] " << t.name << "\n";
        try {
            t.func();
        } catch (const std::exception& e) {
            std::cerr << "  EXCEPTION: " << e.what() << "\n";
            r.failed++;
        } catch (...) {
            std::cerr << "  UNKNOWN EXCEPTION\n";
            r.failed++;
        }
        if (r.failed == before) {
            std::cout << "[       OK ] " << t.name << "\n\n";
            r.passed++;
        } else {
            std::cout << "[  FAILED  ] " << t.name << "\n\n";
        }
    }
    std::cout << "========================================\n";
    std::cout << "Passed: " << r.passed << "\n";
    std::cout << "Failed: " << r.failed << "\n";
    return r.failed == 0 ? 0 : 1;
}

} // namespace test

#define TEST(name) \
    void test_##name(); \
    static test::Register reg_##name(#name, test_##name); \
    void test_##name()

#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "  CHECK failed: " << #cond \
                      << " (line " << __LINE__ << ")\n"; \
            test::Registry::get().failed++; \
        } \
    } while (0)

#define CHECK_FALSE(cond) CHECK(!(cond))

#define CHECK_EQ(a, b) \
    do { \
        auto _va = (a); \
        auto _vb = (b); \
        if (!(_va == _vb)) { \
            std::cerr << "  CHECK_EQ failed: " << #a << " == " << #b \
                      << " (got " << _va << " vs " << _vb << ")" \
                      << " (line " << __LINE__ << ")\n"; \
            test::Registry::get().failed++; \
        } \
    } while (0)
