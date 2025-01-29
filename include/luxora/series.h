#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

namespace Luxora {

const constexpr auto int2string     = [](const int& x) { return std::to_string(x); };
const constexpr auto int64_t2string = [](const int64_t& x) { return std::to_string(x); };
const constexpr auto float2string   = [](const float& x) { return std::to_string(x); };
const constexpr auto double2string  = [](const double& x) { return std::to_string(x); };
const constexpr auto size_t2string  = [](const size_t& x) { return std::to_string(x); };
const constexpr auto string2int     = [](const std::string& x) { return std::stoi(x); };
const constexpr auto string2int64_t = [](const std::string& x) { return (int64_t)std::stol(x); };
const constexpr auto string2float   = [](const std::string& x) { return std::stof(x); };
const constexpr auto string2double  = [](const std::string& x) { return std::stod(x); };
const constexpr auto string2size_t  = [](const std::string& x) { return (size_t)std::stoul(x); };

class SeriesUntyped {
  public:
    virtual ~SeriesUntyped()                   = default;
    virtual std::byte*       data()            = 0;
    virtual const std::byte* data() const      = 0;
    virtual size_t           size() const      = 0;
    virtual std::type_index  type() const      = 0;
    virtual size_t           type_size() const = 0;

    virtual std::optional<std::string> string_at(size_t) const = 0;
};

template <typename T>
class Series : public SeriesUntyped {
    using Element = std::optional<T>;
    using Storage = std::vector<Element>;
    Storage storage;

    std::vector<T> sorted;
    bool           needs_update = true;

  public:
    Series(const Storage&);
    Series(const std::initializer_list<Element>&);

    static Series from_vector(const std::vector<T>&);
    Storage       get_vector() const;

    template <typename U>
    Series(const Series<U>& other) {
        if constexpr (!std::is_same_v<T, U>) {
            throw std::runtime_error("Different types");
        } else {
            storage = other.storage;
        }
    }

    template <typename U>
    Series(const Series<U>&& other) {
        if constexpr (!std::is_same_v<T, U>) {
            throw std::runtime_error("Different types");
        } else {
            storage = std::move(other.storage);
        }
    }

    std::byte* data() override {
        return reinterpret_cast<std::byte*>(storage.data());
    }
    const std::byte* data() const override {
        return reinterpret_cast<const std::byte*>(storage.data());
    }
    size_t size() const override {
        return storage.size();
    }
    std::type_index type() const override {
        return typeid(T);
    }
    size_t type_size() const override {
        return sizeof(T);
    }

    std::optional<std::string> string_at(size_t index) const override {
        if (!storage[index].has_value()) {
            return {};
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return storage[index].value();
        } else {
            return std::to_string(storage[index].value());
        }
    }

    bool operator==(const Series<T>& other) const {
        return storage == other.storage;
    }

    template <typename U>
    Series<U> map(std::function<U(const T&)> f) const {
        std::vector<std::optional<U>> converted(storage.size());
        for (size_t i = 0; i < storage.size(); ++i) {
            if (storage[i].has_value()) {
                converted[i] = f(storage[i].value());
            } else {
                converted[i] = {};
            }
        }
        return Series<U>(converted);
    }

    template <typename U>
    Series<U> map_option(std::function<std::optional<U>(const Element&)> f) const {
        std::vector<std::optional<U>> converted(storage.size());
        for (size_t i = 0; i < storage.size(); ++i) {
            converted[i] = f(storage[i]);
        }
        return Series<U>(converted);
    }

    Series<T> map(std::function<T(const T&)> f) const {
        std::vector<std::optional<T>> converted(storage.size());
        for (size_t i = 0; i < storage.size(); ++i) {
            if (storage[i].has_value()) {
                converted[i] = f(storage[i].value());
            } else {
                converted[i] = {};
            }
        }
        return Series<T>(converted);
    }

    void map_inplace(std::function<T(const T&)> f) {
        for (size_t i = 0; i < storage.size(); ++i) {
            if (storage[i].has_value()) {
                storage[i] = f(storage[i].value());
            } else {
                storage[i] = {};
            }
        }
        needs_update = true;
    }

    void map_inplace_option(std::function<std::optional<T>(const std::optional<T>&)> f) const {
        for (size_t i = 0; i < storage.size(); ++i) {
            storage[i] = f(storage[i]);
        }
    }

    template <typename U>
    Series<U> cast() const {
        if constexpr (std::is_convertible_v<T, U>) {
            std::vector<std::optional<U>> converted(storage.size());
            for (size_t i = 0; i < storage.size(); ++i) {
                if (storage[i].has_value()) {
                    converted[i] = static_cast<U>(storage[i].value());
                } else {
                    converted[i] = {};
                }
            }
            return Series<U>(converted);
        } else {
            throw std::invalid_argument("Incompatible type for easy conversion");
        }
    }

    T sum() const;
    T mean() const;
    T median() const;

    T max() const;
    T min() const;
    T range() const;

    T quantile(float q);
    T iqr();

    T variance() const;
    T stddev() const;

    size_t count() const;

    Series<T> normalized_minmax() const;
    Series<T> normalized_zscore() const;

    std::vector<T>      outliers();
    std::vector<size_t> outlier_indices();

    void identify_na(const T& na);
    void fill_na(const T& fill);

    template <typename T2>
    friend std::ostream& operator<<(std::ostream&, const Series<T2>&);

  private:
    void           sort();
    std::vector<T> filter();
};

template <typename T>
Series<T>::Series(const Storage& storage) : storage(storage) {}

template <typename T>
Series<T>::Series(const std::initializer_list<Element>& init) : storage(init) {}

template <typename T>
Series<T> Series<T>::from_vector(const std::vector<T>& vec) {
    Storage storage(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        storage[i] = std::make_optional(vec[i]);
    }
    return Series(storage);
}

template <typename T>
std::vector<std::optional<T>> Series<T>::get_vector() const {
    return storage;
}

template <typename T>
T Series<T>::mean() const {
    if constexpr (std::is_arithmetic_v<T>) {
        size_t c = count();
        if (c == 0) {
            throw std::logic_error("Not enough non missing values");
        }
        return sum() / count();
    } else {
        throw std::logic_error("Type is not arithmetic");
    }
}

template <typename T>
T Series<T>::sum() const {
    if constexpr (std::is_arithmetic_v<T>) {
        T sum = 0;
        for (const std::optional<T>& x : storage) {
            sum += x.value_or(0);
        }
        return sum;
    } else {
        throw std::logic_error("Type is not arithmetic");
    }
}

template <typename T>
T Series<T>::median() const {
    size_t c = count();
    if (c == 0) {
        throw std::logic_error("Not enough non missing values ");
    }
    if constexpr (std::is_arithmetic_v<T>) {
        std::vector<T> buf(c);
        int            i = 0;
        for (const std::optional<T>& x : storage) {
            if (x.has_value()) {
                buf[i++] = x.value();
            }
        }
        std::sort(buf.rbegin(), buf.rend());
        if (buf.size() % 2 == 1) {
            return buf[buf.size() / 2];
        } else {
            return (buf[buf.size() / 2 - 1] + buf[buf.size() / 2]) / 2;
        }
    } else {
        throw std::logic_error("Type is not arithmetic");
    }
}

template <typename T>
T Series<T>::max() const {
    // TODO comparable
    size_t i = 0;
    while (!storage[i].has_value()) {
        i += 1;
    }
    T res = storage[i].value();
    for (; i < size(); ++i) {
        if (storage[i].has_value() && res < storage[i].value()) {
            res = storage[i].value();
        }
    }
    return res;
}

template <typename T>
T Series<T>::min() const {
    size_t i = 0;
    while (!storage[i].has_value()) {
        i += 1;
    }
    T res = storage[i].value();
    for (; i < size(); ++i) {
        if (storage[i].has_value() && res > storage[i].value()) {
            res = storage[i].value();
        }
    }
    return res;
}

template <typename T>
T Series<T>::range() const {
    return max() - min();
}

template <typename T>
size_t Series<T>::count() const {
    size_t len = 0;
    for (const std::optional<T>& x : storage) {
        len += x.has_value();
    }
    return len;
}

template <typename T>
T Series<T>::quantile(float q) {
    sort();
    size_t index = q * count();
    return sorted[index];
}

template <typename T>
T Series<T>::iqr() {
    return quantile(0.75) - quantile(0.25);
}

template <typename T>
T Series<T>::variance() const {
    if constexpr (std::is_arithmetic_v<T>) {
        T mean_ = mean();
        T res   = 0;
        for (const std::optional<T>& x : storage) {
            if (x.has_value()) {
                res += (x.value() - mean_) * (x.value() - mean_);
            }
        }
        return res / count();
    } else {
        throw std::logic_error("Type is not arithmetic");
    }
}

template <typename T>
T Series<T>::stddev() const {
    return std::sqrt(variance());
}

template <typename T>
Series<T> Series<T>::normalized_minmax() const {
    if constexpr (std::is_arithmetic_v<T>) {
        T min_ = min(), max_ = max();
        T range_ = max_ - min_;
        return map<T>([min_, max_, range_](const T& x) { return (x - min_) / range_; });
    } else {
        throw std::logic_error("Type is not arithmetic");
    }
}

template <typename T>
Series<T> Series<T>::normalized_zscore() const {
    if constexpr (std::is_arithmetic_v<T>) {
        T mean_ = mean(), stddev_ = stddev();
        return map<T>([mean_, stddev_](const T& x) { return (x - mean_) / stddev_; });
    } else {
        throw std::logic_error("Type is not arithmetic");
    }
}

template <typename T>
std::vector<size_t> Series<T>::outlier_indices() {
    T                   q1 = quantile(0.25), q3 = quantile(0.75);
    T                   iqr_        = iqr();
    T                   upper_bound = q3 + 1.5f * iqr_, lower_bound = q1 - 1.5f * iqr_;
    std::vector<size_t> res;
    for (size_t i = 0; i < size(); ++i) {
        if (!storage[i].has_value()) {
            continue;
        }
        if (storage[i].value() > upper_bound || storage[i].value() < lower_bound) {
            res.push_back(i);
        }
    }
    return res;
}

template <typename T>
std::vector<T> Series<T>::outliers() {
    auto           indices = outlier_indices();
    std::vector<T> res(indices.size());
    for (size_t i = 0; i < res.size(); ++i) {
        res[i] = storage[indices[i]].value();
    }
    return res;
}

template <typename T>
void Series<T>::identify_na(const T& na) {
    for (std::optional<T>& x : storage) {
        if (x.has_value() && x.value() == na) {
            x = {};
        }
    }
    needs_update = true;
}

template <typename T>
void Series<T>::fill_na(const T& fill) {
    for (std::optional<T>& x : storage) {
        if (!x.has_value()) {
            x = fill;
        }
    }
    needs_update = true;
}

template <typename T2>
std::ostream& operator<<(std::ostream& os, const Series<T2>& series) {
    os << std::string("Storage: ");
    for (size_t i = 0; i < series.size(); ++i) {
        if (series.storage[i].has_value()) {
            os << series.storage[i].value() << (i + 1 == series.size() ? "" : ", ");
        } else {
            os << std::string("`None`") << (i + 1 == series.size() ? "" : ", ");
        }
    }
    os << std::endl << std::string("Sorted: ");
    for (size_t i = 0; i < series.sorted.size(); ++i) {
        os << series.sorted[i] << (i + 1 == series.size() ? "" : ", ");
    }
    os << std::endl << std::string("Dirty: ") << series.needs_update << std::endl;
    return os;
}

template <typename T>
void Series<T>::sort() {
    if (needs_update) {
        sorted = filter();
        std::sort(sorted.begin(), sorted.end());
        needs_update = false;
    }
}

template <typename T>
std::vector<T> Series<T>::filter() {
    std::vector<T> res(count());
    size_t         j = 0;
    for (size_t i = 0; i < size(); ++i) {
        if (storage[i].has_value()) {
            res[j++] = storage[i].value();
        }
    }
    return res;
}

} // namespace Luxora
