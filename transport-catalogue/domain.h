#pragma once

#include "geo.h"

#include <string>
#include <vector>


struct Stop {
	Stop(const std::string& n, double val1, double val2, size_t id) :name(n), x(val1), y(val2), stop_id(id*2) {}

	std::string name;
	double x = 0;
	double y = 0;
	size_t stop_id = 0;

};

struct Bus {

	Bus(const std::string& n, const std::vector<Stop*>& r, bool c, int bw, double vel) :name(n), route(r), circle(c), bus_wait_time(bw), bus_velocity((vel*1000)/60) {}

	
	std::string name;
	std::vector<Stop*> route;
	bool circle = false;
	int bus_wait_time = 0;
	double bus_velocity = 0;
};
