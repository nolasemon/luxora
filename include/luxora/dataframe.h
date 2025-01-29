#pragma once

#include "luxora/series.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <ostream>
#include <rapidcsv.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Luxora {

#if defined(__GNUG__)
#include <cxxabi.h>
#include <memory>

inline std::string demangle(const char* mangledName) {
    int                                    status = 0;
    std::unique_ptr<char, void (*)(void*)> realname(abi::__cxa_demangle(mangledName, 0, 0, &status), std::free);
    return status == 0 ? realname.get() : mangledName;
}
#else
std::string demangle(const char* mangledName) {
    return mangledName;
}
#endif

inline std::string type_name(std::type_index ti) {
    return demangle(ti.name());
}

using Position = std::pair<size_t, size_t>;

struct PositionHash {
    size_t operator()(const Position& p) const {
        return std::hash<size_t>{}(p.first) ^ std::hash<size_t>{}(p.second);
    }
};

enum Strategy {
    Mean,
    Median,
};

enum NormMethod {
    MinMax,
    Zscore,
};

class DataFrame {
    std::vector<std::unique_ptr<SeriesUntyped>> columns;
    std::unordered_map<std::string, size_t>     column_indices;
    std::vector<std::string>                    column_names;

  public:
    std::pair<size_t, size_t> shape;

  public:
    DataFrame();
    DataFrame(std::string filename);

    void load(std::string filename);
    void load(std::istream& is);
    void save(std::string filename) const;
    void save(std::ostream& os) const;

    std::ostream& choose_rows(std::ostream&, std::vector<size_t> rows) const;

    template <typename T>
    size_t add_column(std::string name);

    template <class U>
    void convert_column(std::string column_name, std::string new_name = "") {
        size_t          column_id = column_indices[column_name];
        std::type_index ti        = columns[column_id]->type();
        // clang-format off
		#define support1(type)                                                                                                 \
			else if (ti == typeid(type)) {                                                                                     \
				convert_column_strictly_typed<type, U>(column_name, new_name);                                                 \
				return;                                                                                                        \
			}
		if constexpr (false) {} 
			support1(int) 
			support1(long long)
			support1(float)
			support1(double)
			support1(size_t)
			support1(std::string)
            // clang-format on

            throw std::invalid_argument("Conversion from `" + type_name(ti) + "` to `" + type_name(typeid(U)) +
                                        "` is not supported.");
    }

    void fill_na(std::string column_name, Strategy strategy = Strategy::Mean);

    template <class T>
    std::vector<T> outliers(std::string column_name);
    template <class T>
    std::vector<size_t> outlier_indices(std::string column_name);

    friend std::ostream& operator<<(std::ostream& out, const DataFrame& df);
    friend bool          operator==(const DataFrame& lhs, const DataFrame& rhs);

    template <class T>
    void normalize(std::string column_name, std::string new_name = "", NormMethod method = MinMax);

  private:
    DataFrame(std::vector<std::string>, std::unordered_map<std::string, size_t>, const std::vector<SeriesUntyped>&);

    void load_from_document(const rapidcsv::Document& document);

    std::ostream& write(std::ostream& os, std::string none) const;
    template <class T>
    Series<T>* get_column(size_t column_id) const {
        if (typeid(T) != columns[column_id]->type()) {
            throw std::invalid_argument("Supplied type differs from original");
        }
        Series<T>* column = dynamic_cast<Series<T>*>(columns[column_id].get());
        if (!column) {
            throw std::bad_cast();
        }
        return column;
    }
    template <class T>
    Series<T>* get_column(std::string column_name) const {
        size_t column_id = column_indices.at(column_name);
        return get_column<T>(column_id);
    }

    template <class T, class U>
    void convert_column_with_conv(std::function<U(const T&)> conv, std::string column_name, std::string new_name = "");
    template <class T, class U>
    void convert_column_easy_conv(std::string column_name, std::string new_name = "");
    template <class T, class U>
    void convert_column_strictly_typed(std::string column_name, std::string new_name = "");

    template <class T>
    void fill_na_typed(std::string column_name, Strategy strategy = Strategy::Mean);
};

template <class T, class U>
void DataFrame::convert_column_with_conv(std::function<U(const T&)> conv, std::string column_name,
                                         std::string new_name) {
    size_t     column_id = column_indices[column_name];
    Series<T>* series    = get_column<T>(column_id);
    if (new_name == "") {
        Series<U> new_series = series->template map<U>(conv);
        columns[column_id]   = std::make_unique<Series<U>>(new_series);
    } else {
        size_t new_column;
        if (column_indices.count(new_name)) {
            new_column = column_indices[new_name];
        } else {
            new_column = add_column<U>(new_name);
        }
        Series<U>* new_series = get_column<U>(new_column);
        *new_series           = series->template map<U>(conv);
    }
}

template <class T, class U>
void DataFrame::convert_column_easy_conv(std::string column_name, std::string new_name) {
    size_t     column_id = column_indices[column_name];
    Series<T>* series    = get_column<T>(column_id);
    if (new_name == "") {
        Series<U> new_series = series->template cast<U>();
        columns[column_id]   = std::make_unique<Series<U>>(new_series);
    } else {
        size_t new_column;
        if (column_indices.count(new_name)) {
            new_column = column_indices[new_name];
        } else {
            new_column = add_column<U>(new_name);
        }
        Series<U>* new_series = get_column<U>(new_column);
        *new_series           = series->template cast<U>();
    }
}

template <class T, class U>
void DataFrame::convert_column_strictly_typed(std::string column_name, std::string new_name) {
    size_t column_id = column_indices[column_name];
    if constexpr (std::is_same_v<T, U>) {
        return;
    } else if constexpr (std::is_convertible_v<T, U>) { // easy conversion
        convert_column_easy_conv<T, U>(column_name, new_name);
        return;
    } else if constexpr (std::is_same_v<U, std::string>) {
        // clang-format off
		#define elseif1(type)                                                                                                  \
			else if constexpr (std::is_same_v<T, type>) {                                                                      \
				convert_column_with_conv<T, U>(type##2string, column_name, new_name);                                          \
				return;                                                                                                        \
			}
		if constexpr (std::is_same_v<T, long long>) {
			convert_column_with_conv<T, U>(int64_t2string, column_name, new_name);
			return;
		}
			elseif1(int)
			elseif1(int64_t)
			elseif1(float)
			elseif1(double)
			elseif1(size_t)
	} else if constexpr (std::is_same_v<T, std::string>) {
		#define elseif2(type)                                                                                                  \
			else if constexpr (std::is_same_v<U, type>) {                                                                      \
				convert_column_with_conv<T, U>(string2##type, column_name, new_name);                                          \
				return;                                                                                                        \
			}
		if constexpr (std::is_same_v<U, long long>) {
			convert_column_with_conv<T, U>(string2int64_t, column_name, new_name);
			return;
		}
			elseif2(int)
			elseif2(int64_t)
			elseif2(float)
			elseif2(double)
			elseif2(size_t)
        // clang-format on
    }
    throw std::invalid_argument("Conversion from `" + type_name(typeid(T)) + "` to `" + type_name(typeid(U)) +
                                "` is not supported.");
}

template <class T>
void DataFrame::fill_na_typed(std::string column_name, Strategy strategy) {
    // TODO: Test
    Series<T>* column = get_column<T>(column_name);
    switch (strategy) {
    case Strategy::Mean:
        column->fill_na(column->mean());
        break;
    case Strategy::Median:
        column->fill_na(column->median());
        break;
    }
}

template <class T>
void DataFrame::normalize(std::string column_name, std::string new_name, NormMethod method) {
    size_t    column_id = column_indices[column_name];
    Series<T>*series    = get_column<T>(column_id), *new_series;
    if (new_name == "") {
        new_series = series;
    } else {
        size_t new_column;
        if (column_indices.count(new_name)) {
            new_column = column_indices[new_name];
        } else {
            new_column = add_column<T>(new_name);
        }
        new_series = get_column<T>(new_column);
    }
    switch (method) {
    case MinMax:
        *new_series = series->normalized_minmax();
        break;
    case Zscore:
        *new_series = series->normalized_zscore();
        break;
    }
}

template <class T>
std::vector<T> DataFrame::outliers(std::string column_name) {
    Series<T>* series = get_column<T>(column_name);
    return series->outliers();
}

template <class T>
std::vector<size_t> DataFrame::outlier_indices(std::string column_name) {
    Series<T>* series = get_column<T>(column_name);
    return series->outlier_indices();
}

template <typename T>
size_t DataFrame::add_column(std::string name) {
    if (std::find(column_names.begin(), column_names.end(), name) != column_names.end()) {
        throw std::invalid_argument("Column with given name already exists");
    }
    columns.push_back(std::make_unique<Series<T>>(Series<T>(std::vector<std::optional<T>>(shape.first))));
    column_names.push_back(name);
    column_indices[name] = columns.size() - 1;
    shape.second += 1;
    return columns.size() - 1;
}

} // namespace Luxora
