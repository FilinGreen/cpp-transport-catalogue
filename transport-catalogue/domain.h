#pragma once

#include "geo.h"

#include <string>
#include <vector>


struct Stop {
	Stop(const std::string& n, double val1, double val2) :name(n), x(val1), y(val2) {}

	std::string name;
	double x = 0;
	double y = 0;

};

struct Bus {
	Bus(const std::string& n, const std::vector<Stop*>& r) :name(n), route(r) {}

	bool circle;
	std::string name;
	std::vector<Stop*> route;
};
