#include <fstream>
#include <istream>
#include <luxora/dataframe.h>
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
		column_names[i]					= document.GetColumnName(i);
		column_indices[column_names[i]] = i;

		Series<std::string> s = document.GetColumn<std::string>(i);
		s.identify_na("");
		columns.push_back(std::make_unique<Series<std::string>>(s));
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
			std::optional<std::string> cell = (*columns[j])[i];
			std::string				   s;
			if (cell.has_value()) {
				s = cell.value();
			} else {
				s = none;
			}
			os << cell.value_or(none) << (j == shape.second - 1 ? '\n' : ',');
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

} // namespace Luxora