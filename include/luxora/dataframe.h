#pragma once

#include "luxora/series.h"
#include <memory>
#include <ostream>
#include <rapidcsv.h>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace Luxora {

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

class DataFrame {
	std::vector<std::unique_ptr<SeriesUntyped>> columns;
	std::unordered_map<std::string, size_t>		column_indices;
	std::vector<std::string>					column_names;

  public:
	std::pair<size_t, size_t> shape;

  public:
	DataFrame();
	DataFrame(std::string filename);

	void load(std::string filename);
	void load(std::istream& is);
	void save(std::string filename) const;
	void save(std::ostream& os) const;

	template <class T>
	void fill_na(std::string column_name, Strategy strategy = Strategy::Mean);

	void normailize(std::string column_name);

	template <class T>
	std::vector<T>		outliers(std::string column_name);
	std::vector<size_t> outlierIndices(std::string column_name);

	friend std::ostream& operator<<(std::ostream& out, const DataFrame& df);
	friend bool			 operator==(const DataFrame& lhs, const DataFrame& rhs);

  private:
	void load_from_document(const rapidcsv::Document& document);

	std::ostream& write(std::ostream& os, std::string none) const;
	template <class T>
	Series<T>* get_column(size_t column_id) {
		Series<T>* column = dynamic_cast<Series<T>*>(columns[column_id].get());
		if (!column) {
			throw std::bad_cast();
		}
		return column;
	}
	template <class T>
	Series<T>* get_column(std::string column_name) {
		size_t column_id = column_indices[column_name];
		return to_series<T>(column_id);
	}
};

template <class T>
void DataFrame::fill_na(std::string column_name, Strategy strategy) {
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

} // namespace Luxora