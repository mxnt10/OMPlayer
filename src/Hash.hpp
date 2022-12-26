#ifndef OMPLAYER_HASH_HPP
#define OMPLAYER_HASH_HPP

#include <iostream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

namespace Hash {
    template<class>
    struct hasher;

    template<>
    struct hasher<std::string> {
        std::size_t constexpr operator()(char const *input) const {
            return *input ? static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) : 5381;
        }

        std::size_t operator()(const std::string &str) const {
            return (*this)(str.c_str());
        }
    };

    template<typename T>
    std::size_t constexpr hash(T &&t) {
        return hasher<typename std::decay<T>::type>()(std::forward<T>(t));
    }

    inline namespace literals {
        std::size_t constexpr operator "" _hash(const char *s, size_t) {
            return hasher<std::string>()(s);
        }
    }
}

#pragma clang diagnostic pop

#endif //OMPLAYER_HASH_HPP
