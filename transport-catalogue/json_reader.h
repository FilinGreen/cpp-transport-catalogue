#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <memory>

#include "geo.h"
#include "transport_catalogue.h"
#include "json.h"
#include "svg.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "ranges.h"
#include "graph.h"
#include "router.h"
#include "transport_router.h"

class JSONReader {
public:
	explicit JSONReader(const json::Node& data);

	void LoadData();
	void ProcessQuery(std::ostream& out);


private:
	const json::Node& data_;
	TransportCatalogue catalog_;
	
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0;

	//--------------------private functions
	void LoadStops(const json::Node& base_data);
	void LoadBuses(const json::Node& base_data, const json::Node& setting_data);


	/*
		void FillGraph(graph::DirectedWeightedGraph< double >& graph);
	void AddBusEdges(graph::DirectedWeightedGraph< double >& graph, Bus& bus, std::vector<Stop* >::iterator begin, std::vector<Stop* >::iterator end);
	void AddCircleBusEdges(graph::DirectedWeightedGraph< double >& graph, Bus& bus);
	void AddLineBusEdges(graph::DirectedWeightedGraph< double >& graph, Bus& bus);
	*/

	


	void ProcessBus(json::Builder& builder, const json::Node& request);
	void ProcessStop(json::Builder& builder, const json::Node& request);
	void ProcessRoute(json::Builder& builder, const json::Node& request, TransportRouter& transport_router);
	void ProcessMap(json::Builder& builder, const json::Node& request);

	renderer::RenderSettings LoadSettings(const json::Node& data);
	renderer::SphereProjector SetProjector(TransportCatalogue& catalog, renderer::RenderSettings& settings);
	svg::Color LoadColor(const json::Node& data);
	svg::Point LoadOffset(const std::vector<json::Node>& data);
};



