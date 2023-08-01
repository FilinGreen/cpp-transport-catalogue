#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <sstream>

#include "geo.h"
#include "transport_catalogue.h"
#include "json.h"
#include "svg.h"
#include "map_renderer.h"

class Reader {
public:
	explicit Reader(const json::Node& data);

	void LoadData();
	void ProcessQuery(std::ostream& out);

private:
	const json::Node& data_;
	TransportCatalogue catalog_;
	//--------------------private functions
	void LoadStops(const json::Node& base_data);
	void LoadBuses(const json::Node& base_data);

	void ProcessBus(std::map<std::string, json::Node>& answer, const json::Node& request);
	void ProcessStop(std::map<std::string, json::Node>& answer, const json::Node& request);
	void ProcessMap(std::map<std::string, json::Node>& answer, const json::Node& request);

};



