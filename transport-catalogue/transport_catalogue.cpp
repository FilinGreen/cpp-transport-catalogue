#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string& name, double x, double y ) {
	if (stopname_to_stop_.count(name) == 0) {
		stops_.push_back({ std::move(name),x,y });
		stopname_to_stop_[stops_.back().name] = &stops_.back();
		stopname_to_buses_[stops_.back().name];
	}
	else {
		stopname_to_stop_.at(name)->x = x;
		stopname_to_stop_.at(name)->y = y;
	}
}

void TransportCatalogue::AddBus(std::string& name, std::vector <std::string>& route) {
	std::vector <Stop*> stops;
	stops.reserve(route.size());

	for (std::string& stopname : route) {
		stops.push_back(stopname_to_stop_.at(stopname));
	}

	buses_.push_back({ std::move(name), std::move(stops) });
	busname_to_bus_[buses_.back().name] = &buses_.back();

	for (auto& stop : buses_.back().route) {
		stopname_to_buses_.at((*stop).name).insert(buses_.back().name);
	}
}

TransportCatalogue::Bus& TransportCatalogue::GetBus(std::string& name) {
	return *busname_to_bus_.at(name);
}

bool TransportCatalogue::Bus_check(std::string& name) {
	return busname_to_bus_.count(name)!=0;
}

bool TransportCatalogue::Stop_check(std::string& name) {
	return stopname_to_stop_.count(name) != 0;
}

std::set<std::string_view>& TransportCatalogue::GetBuses(std::string& name) {
	return stopname_to_buses_.at(name);
}

void TransportCatalogue::AddDistance(std::string& stop1, std::string& stop2, double distance) {// name можно доработать через вью или мув
	
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