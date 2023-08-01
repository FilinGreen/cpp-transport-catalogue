#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

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
