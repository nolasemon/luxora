#pragma once

#include "luxora/series.h"
#include <memory>
#include <ostream>
#include <rapidcsv.h>
#include <string>
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
	std::pair<size_t, size_t>					shape;

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
};

template <class T>
void DataFrame::fill_na(std::string column_name, Strategy strategy) {
	Series<T> column = columns[column_indices[column_name]];
	switch (strategy) {
	case Strategy::Mean:
		column.fill_na(column.mean());
		break;
	case Strategy::Median:
		column.fill_na(column.median());
		break;
	}
	columns[column_indices[column_name]] = column;
}

} // namespace Luxora