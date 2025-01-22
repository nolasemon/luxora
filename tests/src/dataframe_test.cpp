#include <gtest/gtest.h>
#include <luxora/dataframe.h>
#include <luxora/luxora.h>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace Luxora;

TEST(DataFrameTest, NoMissing) {
    ASSERT_NO_THROW(DataFrame df("resources/full.csv"););
    DataFrame df("resources/full.csv");
    ASSERT_EQ(df.shape, std::make_pair(5, 6));
}

TEST(DataFrameTest, Print) {
    DataFrame          df("resources/full.csv");
    std::ostringstream oss;
    ASSERT_NO_THROW(oss << df);

    ASSERT_EQ(oss.str(), "\
Open,High,Low,Close,Volume,Adj Close\n\
64.529999,64.800003,64.139999,64.620003,21705200,64.620003\n\
64.419998,64.730003,64.190002,64.620003,20235200,64.620003\n\
64.330002,64.389999,64.050003,64.360001,19259700,64.360001\n\
64.610001,64.949997,64.449997,64.489998,19384900,64.489998\n\
64.470001,64.690002,64.300003,64.620003,21234600,64.620003\n");
}

TEST(DataFrameTest, MissingData) {
    ASSERT_NO_THROW(DataFrame df("resources/missing.csv"));
    DataFrame df("resources/missing.csv");
    ASSERT_EQ(df.shape, std::make_pair(5, 6));
}

TEST(DataFrameTest, PrintMissing) {
    DataFrame          df("resources/missing.csv");
    std::ostringstream oss;
    ASSERT_NO_THROW(oss << df);

    ASSERT_EQ(oss.str(), "\
Open,High,Low,Close,Volume,Adj Close\n\
64.529999,64.800003,64.139999,64.620003,21705200,64.620003\n\
64.419998,64.730003,64.190002,64.620003,20235200,64.620003\n\
64.330002,64.389999,64.050003,64.360001,19259700,64.360001\n\
64.610001,64.949997,64.449997,64.489998,19384900,64.489998\n\
64.470001,64.690002,64.300003,`None`,21234600,64.620003\n");
}

TEST(DataFrameTest, MultipleLoadTest) {
    DataFrame df;
    ASSERT_EQ(df.shape, std::make_pair(0, 0));
    df.load("resources/full.csv");
    ASSERT_EQ(df.shape, std::make_pair(5, 6));
    std::ostringstream oss;
    ASSERT_NO_THROW(oss << df);

    ASSERT_EQ(oss.str(), "\
Open,High,Low,Close,Volume,Adj Close\n\
64.529999,64.800003,64.139999,64.620003,21705200,64.620003\n\
64.419998,64.730003,64.190002,64.620003,20235200,64.620003\n\
64.330002,64.389999,64.050003,64.360001,19259700,64.360001\n\
64.610001,64.949997,64.449997,64.489998,19384900,64.489998\n\
64.470001,64.690002,64.300003,64.620003,21234600,64.620003\n");
    oss.str("");
    oss.clear();
    df.load("resources/missing.csv");
    ASSERT_EQ(df.shape, std::make_pair(5, 6));
    oss << df;
    ASSERT_EQ(oss.str(), "\
Open,High,Low,Close,Volume,Adj Close\n\
64.529999,64.800003,64.139999,64.620003,21705200,64.620003\n\
64.419998,64.730003,64.190002,64.620003,20235200,64.620003\n\
64.330002,64.389999,64.050003,64.360001,19259700,64.360001\n\
64.610001,64.949997,64.449997,64.489998,19384900,64.489998\n\
64.470001,64.690002,64.300003,`None`,21234600,64.620003\n");
}

TEST(DataFrameTest, SaveTest) {
    std::ostringstream oss;
    DataFrame          df("resources/missing.csv");
    EXPECT_NO_THROW(df.save(oss));

    std::istringstream iss(oss.str());

    DataFrame df2;
    df2.load(iss);
    ASSERT_EQ(df, df2);
}

TEST(DataFrameTest, AddColumnTest) {
    DataFrame df("resources/missing.csv");
    ASSERT_EQ(df.shape, std::make_pair(5, 6));
    df.add_column<float>("float_column");

    ASSERT_EQ(df.shape, std::make_pair(5, 7));

    // Same name
    ASSERT_THROW(df.add_column<float>("float_column"), std::invalid_argument);
}

TEST(DataFrameTest, TestConvertColumn) {
    DataFrame df("resources/missing.csv");
    df.convert_column<int>("High");

    // Same name
    ASSERT_THROW(df.convert_column<long long>("High", "High"), std::invalid_argument);
    df.convert_column<long long>("Low", "long_column");

    // No conversion needed string->string
    ASSERT_THROW(df.convert_column<std::string>("Adj Close"), std::invalid_argument);
    df.convert_column<float>("Volume");
    df.convert_column<size_t>("Volume", "new volume");
    std::ostringstream oss;
    oss << df;
    ASSERT_EQ(oss.str(), "\
Open,High,Low,Close,Volume,Adj Close,long_column,new volume\n\
64.529999,64,64.139999,64.620003,21705200.000000,64.620003,64,21705200\n\
64.419998,64,64.190002,64.620003,20235200.000000,64.620003,64,20235200\n\
64.330002,64,64.050003,64.360001,19259700.000000,64.360001,64,19259700\n\
64.610001,64,64.449997,64.489998,19384900.000000,64.489998,64,19384900\n\
64.470001,64,64.300003,`None`,21234600.000000,64.620003,64,21234600\n");
}

TEST(DataFrameTest, FillNaTest) {
    DataFrame df("resources/missing.csv");
    df.convert_column<float>("Close");
    df.fill_na("Close", Strategy::Median);
    std::ostringstream oss;
    oss << df;
    ASSERT_EQ(oss.str(), "Open,High,Low,Close,Volume,Adj Close\n\
64.529999,64.800003,64.139999,64.620003,21705200,64.620003\n\
64.419998,64.730003,64.190002,64.620003,20235200,64.620003\n\
64.330002,64.389999,64.050003,64.360001,19259700,64.360001\n\
64.610001,64.949997,64.449997,64.489998,19384900,64.489998\n\
64.470001,64.690002,64.300003,64.555000,21234600,64.620003\n");
}
