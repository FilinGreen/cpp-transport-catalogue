#pragma once

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <utility>
#include <iostream>
#include <set>
#include <utility>
#include <functional>

#include "geo.h"
#include "domain.h"
#include "json.h"


class TransportCatalogue {
public:

	class StopsPairHasher {
	public:
		size_t operator()(const std::pair<Stop*, Stop*>& couple) const {
			std::hash<const void*> stop;
			return stop(couple.first) * 1000 + stop(couple.second);;
		}
	};

	void AddStop(const std::string& name, double x, double y);
	Stop& GetStop(const std::string& name);
	std::deque <Stop>& GetStops();
	bool HasStop(const std::string& name);
	bool HasBusAtStop(const std::string& name);

	void AddBus(const std::string& name, const std::vector <std::string>& route);
	void AddBus(const std::string& name, const std::vector <std::string>& route, bool circle);
	Bus& GetBus(const std::string& name);
	std::deque <Bus>& GetBuses();
	bool HasBus(const std::string& name);	
	bool IsCircle(const std::string& name);
	std::set<std::string_view>& GetBuses(const std::string& name);


	void AddDistance(const std::string& stop1, const std::string& stop2, double distance);
	double GetDistance(Stop* stop1, Stop* stop2);



private:
	std::deque <Stop> stops_;
	std::unordered_map <std::string_view, Stop* > stopname_to_stop_;
	std::deque <Bus> buses_;
	std::unordered_map <std::string_view, Bus*> busname_to_bus_;
	std::unordered_map< std::string_view, std::set<std::string_view>> stopname_to_buses_;
	std::unordered_map<std::pair<Stop*, Stop*>, double, StopsPairHasher> real_distance_;

};