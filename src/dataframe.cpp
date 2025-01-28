#include "luxora/fixed_string.h"
#include <fstream>
#include <istream>
#include <luxora/dataframe.h>
#include <luxora/series.h>
#include <memory>
#include <rapidcsv.h>
#include <string>
#include <unordered_map>

namespace Luxora {

DataFrame::DataFrame() {}
DataFrame::DataFrame(std::string filename) {
    load(filename);
}

void DataFrame::load_from_document(const rapidcsv::Document& document) {
    column_indices.clear();
    columns.clear();
    shape.second = document.GetColumnCount();
    column_names.resize(shape.second);
    column_indices.reserve(shape.second);
    for (size_t i = 0; i < shape.second; ++i) {
        column_names[i]                 = document.GetColumnName(i);
        column_indices[column_names[i]] = i;

        Series s = Series<String>::from_vector(document.GetColumn<std::string>(i));
        s.identify_na(String(""));
        columns.push_back(std::make_unique<Series<String>>(s));
    }
    shape.first = columns[0]->size();
}

void DataFrame::load(std::string filename) {
    rapidcsv::Document document(filename);
    load_from_document(document);
}

void DataFrame::load(std::istream& is) {
    rapidcsv::Document document(is);
    load_from_document(document);
}

std::ostream& DataFrame::write(std::ostream& os, std::string none = "") const {
    for (size_t j = 0; j < shape.second; ++j) {
        os << column_names[j] << (j == shape.second - 1 ? '\n' : ',');
    }
    for (size_t i = 0; i < shape.first; ++i) {
        for (size_t j = 0; j < shape.second; ++j) {
            std::optional<std::string> cell = columns[j]->string_at(i);
            os << cell.value_or(none) << (j == shape.second - 1 ? '\n' : ',');
        }
    }
    return os;
}

std::ostream& DataFrame::choose_rows(std::ostream& os, std::vector<size_t> indices) const {
    for (size_t j = 0; j < shape.second; ++j) {
        os << column_names[j] << (j == shape.second - 1 ? '\n' : ',');
    }
    for (auto i : indices) {
        for (size_t j = 0; j < shape.second; ++j) {
            std::optional<std::string> cell = columns[j]->string_at(i);
            os << cell.value_or("`None`") << (j == shape.second - 1 ? '\n' : ',');
        }
    }
    return os;
}

void DataFrame::save(std::string filename) const {
    std::ofstream file(filename);
    save(file);
}

void DataFrame::save(std::ostream& os) const {
    write(os);
}

std::ostream& operator<<(std::ostream& out, const DataFrame& df) {
    df.write(out, "`None`");
    return out;
}

bool operator==(const DataFrame& lhs, const DataFrame& rhs) {
    if (lhs.column_names != rhs.column_names || lhs.shape != rhs.shape) {
        return false;
    }
    for (size_t j = 0; j < lhs.shape.second; ++j) {
        if (*(lhs.columns[j]) != *(rhs.columns[j])) {
            return false;
        }
    }
    return true;
}

void DataFrame::fill_na(std::string column_name, Strategy strategy) {
    size_t          column_id = column_indices[column_name];
    std::type_index ti        = columns[column_id]->type();
    // clang-format off
		#define support2(type)                                                                                                 \
			else if (ti == typeid(type)) {                                                                                     \
				fill_na_typed<type>(column_name, strategy);\
				return;                                                                                                        \
			}
		if constexpr (false) {} 
			support2(int) 
			support2(long long)
			support2(float)
			support2(double)
			support2(size_t)
			support2(String)
        // clang-format on
        throw std::invalid_argument("Imputation of column " + column_name + " with type " + type_name(ti) +
                                    " is not supported.");
}

} // namespace Luxora
