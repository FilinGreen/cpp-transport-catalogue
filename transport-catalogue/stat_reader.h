#pragma once

#include <iostream>
#include <unordered_set>
#include <algorithm>

#include "geo.h"
#include "transport_catalogue.h"
namespace Stats {
	void ExecuteQueries(TransportCatalogue& catalog, std::istream& input, std::ostream& out);

	void ProcessBus(TransportCatalogue& catalog, std::string& text, std::ostream& out);

	void ProcessStop(TransportCatalogue& catalog, std::string& text, std::ostream& out);
}//namespace Stats