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

class JSONReader {
public:
	explicit JSONReader(const json::Node& data);

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

	renderer::RenderSettings LoadSettings(const json::Node& data);
	renderer::SphereProjector SetProjector(TransportCatalogue& catalog, renderer::RenderSettings& settings);
	svg::Color LoadColor(const json::Node& data);
	svg::Point LoadOffset(const std::vector<json::Node>& data);
};



