#include "luxora/dataframe.h"
#include <CLI11.hpp>
#include <iostream>
#include <luxora/luxora.h>
#include <map>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

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

    CLI::App*   load = app.add_subcommand("load");
    std::string filename;
    load->add_option("filename", filename, "File with data")->required()->check(CLI::ExistingFile);

    CLI::App*   save   = app.add_subcommand("save");
    std::string output = "output.csv";
    save->add_option("output", output, "File to save data")->default_val(output);

    CLI::App* exit = app.add_subcommand("exit");

    CLI::App*   column_from = app.add_subcommand("from");
    std::string column_from_name;
    column_from->add_option("from", column_from_name, "Name of a column to work with")->required();

    CLI::App*   column_to      = app.add_subcommand("to");
    std::string column_to_name = "";
    column_to->add_option("to", column_to_name, "Name of a column to store result")->default_val("");

    CLI::App* impute = app.add_subcommand("impute", "Imputes missing data inplace.");
    Strategy  strategy;
    impute->add_option("-s,--strat", strategy, "Imputation strategy")
        ->transform(CLI::CheckedTransformer(
            std::map<std::string, Strategy>{{"mean", Strategy::Mean}, {"median", Strategy::Median}}))
        ->required();

    CLI::App* normalize = app.add_subcommand("normalize", "Normalize column");
    bool      zscore    = 0;
    normalize->add_flag("--zscore", zscore, "Set normalization method to Z-score instead of MinMax");

    CLI::App* outliers  = app.add_subcommand("outliers", "Detect outliers");
    bool      show_rows = false;
    outliers->add_flag("--rows", show_rows, "Show table rows instead of values");

    app.require_subcommand(1, 1);

    DataFrame df;

    bool        loaded = false;
    std::string line;
    while (true) {
        std::cout << ">>> " << std::flush;
        if (!std::getline(std::cin, line)) {
            break;
        }
        try {
            app.parse(line);
        } catch (const CLI::ParseError& e) {
            std::cerr << "An error has occured: \n" << e.what() << std::endl;
            continue;
        }

        if (load->parsed()) {
            df.load(filename);
        } else if (save->parsed()) {
            df.save(output);
        } else if (exit->parsed()) {
            break;
        } else if (impute->parsed()) {
            df.convert_column<float>(column_from_name);
            df.fill_na(column_from_name, strategy);
        } else if (normalize->parsed()) {
            df.convert_column<float>(column_from_name);
            if (zscore) {
                df.normalize<float>(column_from_name, column_to_name, Luxora::Zscore);
            } else {
                df.normalize<float>(column_from_name, column_to_name);
            }
        } else if (outliers->parsed()) {
            df.convert_column<float>(column_from_name);
            std::cout << "Outliers: ";
            if (show_rows) {
                auto indices = df.outlier_indices<float>(column_from_name);
                std::cout << std::endl;
                df.choose_rows(std::cout, indices);
            } else {
                auto values = df.outliers<float>(column_from_name);
                std::cout << values << std::endl;
            }
        }
    }

    return 0;
}
