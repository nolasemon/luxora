#include <gtest/gtest.h>
#include <luxora/dataframe.h>
#include <luxora/luxora.h>
#include <sstream>
#include <utility>

using namespace Luxora;

TEST(DataFrameTest, NoMissing) {
	ASSERT_NO_THROW(DataFrame df("resources/full.csv"););
	DataFrame df("resources/full.csv");
	ASSERT_EQ(df.shape, std::make_pair(5, 6));
}

TEST(DataFrameTest, Print) {
	DataFrame		   df("resources/full.csv");
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
	DataFrame		   df("resources/missing.csv");
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
	DataFrame		   df("resources/missing.csv");
	EXPECT_NO_THROW(df.save(oss));

	std::istringstream iss(oss.str());

	DataFrame df2;
	df2.load(iss);
	ASSERT_EQ(df, df2);
}