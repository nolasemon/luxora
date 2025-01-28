#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Luxora {

template <size_t N>
class FixedString : public std::array<char, N> {
  public:
    FixedString(const char* string) {
        size_t len = strlen(string);
        if (len >= N) {
            throw std::invalid_argument("String size " + std::to_string(len) +
                                        " is too big for N=" + std::to_string(N));
        }
        this->fill(0);
        std::strncpy(this->data(), string, len);
    }

    FixedString(const std::string& string) {
        size_t len = string.size();
        if (len >= N) {
            throw std::invalid_argument("String size " + std::to_string(len) +
                                        " is too big for N=" + std::to_string(N));
        }
        this->fill(0);
        std::strncpy(this->data(), string.data(), len);
    }

    FixedString(std::string&& string) {
        size_t len = string.size();
        if (len >= N) {
            throw std::invalid_argument("String size " + std::to_string(len) +
                                        " is too big for N=" + std::to_string(N));
        }
        this->fill(0);
        std::strncpy(this->data(), std::move(string.data()), len);
    }

    template <size_t M>
    FixedString(FixedString<M> string) {
        size_t len = string.len();
        if (len >= N) {
            throw std::invalid_argument("String size " + std::to_string(len) +
                                        " is too big for N=" + std::to_string(N));
        }
        this->fill(0);
        std::strncpy(this->data(), string.data(), len);
    }

    std::string to_string() const;

    int to_int() const {
        return atoi(this->data());
    }
    long long to_long() const {
        return atoll(this->data());
    }
    size_t to_size() const {
        return (size_t)atol(this->data());
    }
    float to_float() const {
        return atof(this->data());
    }
    double to_double() const {
        return atof(this->data());
    }

    size_t len() const {
        return strlen(this->data());
    }

    template <size_t M>
    friend std::ostream& operator<<(std::ostream&, FixedString<M>);
};

using MiniString = FixedString<16>;
using String     = FixedString<64>;

template <size_t N>
std::string FixedString<N>::to_string() const {
    return std::string(this->data());
}

template <size_t M>
std::ostream& operator<<(std::ostream& out, FixedString<M> string) {
    out << string.data();
    return out;
}

template <typename T>
String to_string(T x)
    requires std::is_integral_v<T> || std::is_floating_point_v<T>
{
    return String(std::to_string(x));
}

}; // namespace Luxora
