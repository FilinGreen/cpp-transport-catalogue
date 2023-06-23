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



class TransportCatalogue {
public:

	struct Stop {
		Stop(const std::string& n, double val1, double val2) :name(n), x(val1), y(val2) {}

		std::string name;
		double x = 0;
		double y = 0;

	};

	struct Bus {
		Bus(const std::string& n, const std::vector<Stop*>& r) :name(n), route(r) {}

		std::string name;
		std::vector<Stop*> route;
	};

	class StopsPairHasher {
	public:
		size_t operator()(const std::pair<TransportCatalogue::Stop*, TransportCatalogue::Stop*>& couple) const {
			std::hash<const void*> stop;
			return stop(couple.first) * 1000 + stop(couple.second);;
		}
	};

	void AddStop(const std::string& name, double x, double y);
	void AddBus(const std::string& name, const std::vector <std::string>& route);
	Bus& GetBus(const std::string& name);
	bool HasBus(const std::string& name);
	bool HasStop(const std::string& name);
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