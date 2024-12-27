#include "luxora/dataframe.h"
#include <CLI11.hpp>
#include <iostream>
#include <luxora/luxora.h>
#include <map>
#include <optional>
#include <stdexcept>

using namespace Luxora;

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
	out << '[';
	for (size_t i = 0; i < v.size(); ++i) {
		out << v[i] << (i + 1 == v.size() ? "" : ", ");
	}
	out << "]";
	return out;
}

int main(int argc, char** argv) {
	CLI::App app{"CLI tool for data preparation."};
	app.set_help_all_flag("--help-all", "Expand all help");
	argv = app.ensure_utf8(argv);

	std::string filename;
	app.add_option("-f,--file", filename, "File with data")->required();

	std::string output = "output.csv";
	app.add_option("-o,--out", output, "File to save data")->default_val(output);

	std::string column_name;
	app.add_option("--column-name", column_name, "Name of a column to work with")->required();

	std::string new_column = "";
	app.add_option("--new-column", new_column, "Name of a column to store result")->default_val("");

	CLI::App* impute = app.add_subcommand("impute", "Imputes missing data inplace.");
	Strategy  strategy;
	impute->add_option("-s,--strat", strategy, "Imputation strategy")
		->transform(CLI::CheckedTransformer(
			std::map<std::string, Strategy>{{"mean", Strategy::Mean}, {"median", Strategy::Median}}))
		->required();

	CLI::App* normalize = app.add_subcommand("normalize", "Normalize column");
	bool	  zscore	= 0;
	normalize->add_flag("--zscore", zscore, "Set normalization method to Z-score instead of MinMax");

	CLI::App* outliers	= app.add_subcommand("outliers", "Detect outliers");
	bool	  show_rows = false;
	outliers->add_flag("--rows", show_rows, "Show table rows instead of values");

	app.require_subcommand(1, 1);

	CLI11_PARSE(app, argc, argv);

	DataFrame df(filename);

	if (impute->parsed()) {
		df.convert_column<float>(column_name);
		df.fill_na(column_name, strategy);
		df.save(output);
	} else if (normalize->parsed()) {
		df.convert_column<float>(column_name);
		if (zscore) {
			df.normalize<float>(column_name, new_column, Luxora::Zscore);
		} else {
			df.normalize<float>(column_name, new_column);
		}
		df.save(output);
	} else if (outliers->parsed()) {
		df.convert_column<float>(column_name);
		std::cout << "Outliers: ";
		if (show_rows) {
			auto indices = df.outlier_indices<float>(column_name);
			std::cout << std::endl;
			df.choose_rows(std::cout, indices);
		} else {
			auto values = df.outliers<float>(column_name);
			std::cout << values << std::endl;
		}
	} else {
		throw std::runtime_error("Unreachable");
	}

	return 0;
}
