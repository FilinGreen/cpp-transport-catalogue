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

namespace Input {
	void LoadTC(TransportCatalogue& result, std::istream& input);

	std::vector<std::string> SplitInToWordsStop(std::string& text);

	std::vector<std::string> SplitIntoWordsBus(std::string& text, char lit);

	void AddStop(TransportCatalogue& catalog, std::string& text);

	void AddBus(TransportCatalogue& catalog, std::string& text);
}//namespace Input

//-----------------------------------------Stats-----------------------------------------------------------------------

namespace Stats {
	void ExecuteQueries(TransportCatalogue& catalog, std::istream& input, std::ostream& out);//

	void ProcessBus(TransportCatalogue& catalog, std::string& text, std::ostream& out);

	void ProcessStop(TransportCatalogue& catalog, std::string& text, std::ostream& out);
}//namespace Stats



namespace Reader {

	void LoadStops(TransportCatalogue& catalog, json::Node base_data);
	void LoadBuses(TransportCatalogue& catalog, json::Node base_data);
	void LoadData(TransportCatalogue& catalog, json::Node data);

	void BusProcessing(TransportCatalogue& catalog, std::map<std::string, json::Node>& answer, json::Node request);
	void StopProcessing(TransportCatalogue& catalog, std::map<std::string, json::Node>& answer, json::Node request);
	void MapProcessing(TransportCatalogue& catalog, std::map<std::string, json::Node>& answer, json::Node request, json::Node data);
	void QueryProcessing(TransportCatalogue& catalog, json::Node data, std::ostream& out);

	svg::Point LoadOffset(std::vector<json::Node> data);
	svg::Color LoadColor(json::Node data);
	renderer::Render_settings LoadSettings(json::Node data);
	renderer::SphereProjector SetProjector(TransportCatalogue& catalog, renderer::Render_settings& settings);

	void DrowLine(svg::Document& doc, renderer::Render_settings& settings, renderer::SphereProjector& proj, int color, std::vector<Stop*>& route);
	void DrowBusname(TransportCatalogue& catalog, std::string busname, svg::Document& doc, renderer::Render_settings& settings, renderer::SphereProjector& proj, int color);
	void DrowStopName(svg::Document& doc, geo::Coordinates coordinates, renderer::Render_settings& settings, renderer::SphereProjector& proj, std::string stopname);
	void DrowStops(svg::Document& doc, geo::Coordinates coordinates, renderer::SphereProjector& proj, renderer::Render_settings& settings);
	void Drow(TransportCatalogue& catalog, renderer::Render_settings& settings, std::ostream& out, renderer::SphereProjector& proj);
	void GraphicProcessing(TransportCatalogue& catalog, json::Node data, std::ostringstream& out);
}//namespace Reader