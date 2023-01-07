#ifdef __TEST_HEAD
#   error "test.hpp header can't be included several times"
#else
#   define __TEST_HEAD
#endif

// LCOV_EXCL_START

#include <string>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>

namespace std {
    inline string to_string(const std::string & str) {
        return str;
    }
}

namespace tests {

class assertion_error : public std::runtime_error {
public:
    explicit assertion_error(const std::string & message) : std::runtime_error(message) {}
};

struct {
    int success = 0;
    template <typename first, typename second>
    void assert_eq(const first & expect, const second & actual, const char * file, std::size_t line) {
        if (actual != expect) {
            std::string message = std::string(file) + ":" + std::to_string(line) + ": assertion failed (\"" + std::to_string(expect) +
            "\" expected, got \"" + std::to_string(actual) + "\")";
            std::cout << "test failed: " << message << std::endl;
            throw assertion_error(message);
        } else
            success++;
    }
    template <typename first, typename second>
    void assert_ne(const first & expect, const second & actual, const char * file, std::size_t line) {
        if (actual == expect) {
            std::string message = std::string(file) + ":" + std::to_string(line) + ": assertion failed (\"" + std::to_string(expect) +
            "\" equals to \"" + std::to_string(actual) + "\")";
            std::cout << "test failed: " << message << std::endl;
            throw assertion_error(message);
        } else
            success++;
    }
    template <typename E, typename F>
    void assert_ex_with(F fun, const char * file, std::size_t line) {
        try {
            fun();
        } catch (const E & e) {
            success++;
            return;
        } catch (...) {
            std::cout << "test failed: " << std::string(file) << ':' << std::to_string(line)
                << ": cought unexpected exception type." << std::endl;
            throw;
        }
        std::string message = std::string(file) + ":" + std::to_string(line) + ": no exception cought.";
        std::cout << "test failed: " << message << std::endl;
        throw assertion_error(message);
    }
    template <typename F>
    void assert_ex(F fun, const char * file, std::size_t line) {
        try {
            fun();
        } catch (...) {
            success++;
            return;
        }
        std::string message = std::string(file) + ":" + std::to_string(line) + ": no exception cought.";
        std::cout << "test failed: " << message << std::endl;
        throw assertion_error(message);
    }
    void assert_tr(bool value, const char * file, std::size_t line, const char * expression) {
        if (value) {
            success++;
        } else {
            std::string message = std::string(file) + ":" + std::to_string(line) + ": value (" + expression + ") was false.";
            std::cout << "test failed: " << message << std::endl;
            throw assertion_error(message);
        }
    }
    void assert_fa(bool value, const char * file, std::size_t line, const char * expression) {
        if (!value) {
            success++;
        } else {
            std::string message = std::string(file) + ":" + std::to_string(line) + ": value (" + expression + ") was true.";
            std::cout << "test failed: " << message << std::endl;
            throw assertion_error(message);
        }
    }
} assert;

#define test \
        void test_function()

#define assert_equals(expect, actual) \
        ::tests::assert.assert_eq(expect, actual, __FILE__, __LINE__)

#define assert_not_equals(expect, actual) \
        ::tests::assert.assert_ne(expect, actual, __FILE__, __LINE__)

#define assert_fails_with(E, fun) \
        ::tests::assert.assert_ex_with<E>([&]() fun, __FILE__, __LINE__)

#define assert_fails(fun) \
        ::tests::assert.assert_ex([&]() fun, __FILE__, __LINE__)

#define assert_true(value) \
        ::tests::assert.assert_tr(value, __FILE__, __LINE__, #value)

#define assert_false(value) \
        ::tests::assert.assert_fa(value, __FILE__, __LINE__, #value)

}

void test_function();

int main() {
    std::cout << "entering test..." << std::endl;
    test_function();
    std::cout << "success: " << tests::assert.success << " assertions passed";
}

// LCOV_EXCL_STOP
