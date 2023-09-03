#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <optional>

#include "ranges.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"
#include "json_builder.h"


struct edgeinfo {
	edgeinfo(std::string& n, int s, size_t i) :name(n), stop_count(s), stopid(i) {}

	std::string name;
	int stop_count = 0;
	size_t stopid = 0;
};

class TransportRouter {
public:
	explicit TransportRouter(TransportCatalogue& catalog, int bt, double bv);

	void ProcessRoute(json::Builder& builder, const json::Node& request, size_t from, size_t to);

private:
	TransportCatalogue& catalog_;
	std::unique_ptr <graph::DirectedWeightedGraph< double >> graph_;
	std::unique_ptr <graph::Router <double>> router_;

	std::map <graph::EdgeId, std::unique_ptr<edgeinfo>> edge_info_;
	double bus_wait_time_ = 0;
	double bus_velocity_ = 0;

	//------------------------private functions
	void FillGraph();
	void AddBusEdges(Bus& bus, std::vector<Stop* >::iterator begin, std::vector<Stop* >::iterator end);
	void AddCircleBusEdges(Bus& bus);
	void AddLineBusEdges(Bus& bus);


};