#include <CLI11.hpp>
#include <iostream>
#include <luxora/luxora.h>
#include <ostream>
#include <rapidcsv.h>

int main(int, char **) {
	std::cout << "Hello, from luxora!" << std::endl;
	std::cout << Luxora::X << " by the way!" << std::endl;
}
