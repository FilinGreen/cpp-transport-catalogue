#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& name, double x, double y ) {
	if (stopname_to_stop_.count(name) == 0) {
		stops_.push_back({name,x,y });
		stopname_to_stop_[stops_.back().name] = &stops_.back();
		stopname_to_buses_[stops_.back().name];
	}
	else {
		stopname_to_stop_.at(name)->x = x;
		stopname_to_stop_.at(name)->y = y;
	}
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector < std::string>& route) {
	std::vector <Stop*> stops;
	stops.reserve(route.size());

	for (const std::string& stopname : route) {
		stops.push_back(stopname_to_stop_.at(stopname));
	}

	buses_.push_back({name, stops });
	busname_to_bus_[buses_.back().name] = &buses_.back();

	for (const auto& stop : buses_.back().route) {
		stopname_to_buses_.at((*stop).name).insert(buses_.back().name);
	}
}

TransportCatalogue::Bus& TransportCatalogue::GetBus(const std::string& name) {
	return *busname_to_bus_.at(name);
}

bool TransportCatalogue::HasBus(const std::string& name) {
	return busname_to_bus_.count(name)!=0;
}

bool TransportCatalogue::HasStop(const std::string& name) {
	return stopname_to_stop_.count(name) != 0;
}

std::set<std::string_view>& TransportCatalogue::GetBuses(const std::string& name) {
	return stopname_to_buses_.at(name);
}

void TransportCatalogue::AddDistance(const std::string& stop1, const std::string& stop2, double distance) {
	
	if (stopname_to_stop_.count(stop2) == 0) {
		std::string stop2copy = stop2;
		AddStop(stop2, 0.0, 0.0);
		real_distance_[{stopname_to_stop_.at(stop1), stopname_to_stop_.at(stop2copy)}] = distance;
	}
	else {
		real_distance_[{stopname_to_stop_.at(stop1), stopname_to_stop_.at(stop2)}] = distance;
	}
}

double TransportCatalogue::GetDistance(Stop* stop1, Stop* stop2) {
	if(real_distance_.count({ stop1,stop2 })!=0){
		return real_distance_.at({ stop1,stop2 });
	}
	else {
		return real_distance_.at({ stop2,stop1 });
	}
	
}