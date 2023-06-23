#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "transport_catalogue.h"

namespace Input {
	void LoadTC(TransportCatalogue& result, std::istream& input);

	std::vector<std::string> SplitInToWordsStop(std::string& text);

	std::vector<std::string> SplitIntoWordsBus(std::string& text, char lit);

	void AddStop(TransportCatalogue& catalog, std::string& text);

	void AddBus(TransportCatalogue& catalog, std::string& text);
}//namespace Input