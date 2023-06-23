#include <iostream>
#include <iomanip>
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main() {
	std::cout << std::setprecision(6);
	TransportCatalogue catalog;
	Input::LoadTC(catalog, std::cin);
	Stats::ExecuteQueries(catalog, std::cin, std::cout);
}