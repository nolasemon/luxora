#include <luxora/luxora.h>

using namespace Luxora;

int main(int, char **) {
	DataFrame		   df("resources/missing.csv");
	std::ostringstream oss;
	oss << df;

	std::string left = oss.str();
	assert(left == "\
Open,High,Low,Close,Volume,Adj Close\n\
64.529999,64.800003,64.139999,64.620003,21705200,64.620003\n\
64.419998,64.730003,64.190002,64.620003,20235200,64.620003\n\
64.330002,64.389999,64.050003,64.360001,19259700,64.360001\n\
64.610001,64.949997,64.449997,64.489998,19384900,64.489998\n\
64.470001,64.690002,64.300003,`None`,21234600,64.620003\n");
}
