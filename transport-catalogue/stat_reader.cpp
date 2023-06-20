#include "stat_reader.h"

namespace Stats {
	void Query_processing(TransportCatalogue& catalog, std::istream& input) {
		int queries_number = 0;
		input >> queries_number;
		std::string text;
		std::getline(input, text);//По какой то причине после того как считываю гетлайном количество команд, следующий гетлайн идет пустой

		for (int i = 0; i < queries_number; ++i) {
			std::getline(input, text);
			if (text.at(0) == 'S') {
				Single_query_processingStop(catalog, text);
			}
			else {
				Single_query_processingBus(catalog, text);
			}

		}
	}

	void Single_query_processingBus(TransportCatalogue& catalog, std::string& text) {

		std::string name = text.substr(text.find(' ') + 1);                       // Именя маршрута
		if (!catalog.Bus_check(name)) {
			std::cout << "Bus " << name << ": not found" << std::endl;
			return;
		}

		double distance = 0;                                                      // Общая географическая дистанция
		double real_distance = 0;                                                 // Общая реальная длина
		std::unordered_set <std::string_view> unique_stops;                       // Сет уникальных остановок
		TransportCatalogue::Stop* prev_stop = catalog.GetBus(name).route.at(0);    // Предыдущая остановка
		bool flag = true;
		for (auto& stop : catalog.GetBus(name).route) {
			unique_stops.insert((*stop).name);
			if (flag) {
				flag = false;

				continue;
			}
			distance += ComputeDistance({ (*prev_stop).x,(*prev_stop).y }, { (*stop).x,(*stop).y });
			real_distance += catalog.GetDistance(prev_stop, stop);
			prev_stop = stop;

		}

		std::cout << "Bus " << name << ": " << catalog.GetBus(name).route.size() << " stops on route, " << unique_stops.size() << " unique stops, " << real_distance << " route length, " << real_distance / distance << " curvature" << std::endl;

	}


	void Single_query_processingStop(TransportCatalogue& catalog, std::string& text) {
		std::string name = text.substr(text.find(' ') + 1);
		std::cout << "Stop " << name << ":";

		if (!catalog.Stop_check(name)) {
			std::cout << " not found" << std::endl;
			return;
		}
		std::set<std::string_view> setbus = catalog.GetBuses(name);
		if (setbus.size() == 0) {
			std::cout << " no buses" << std::endl;
			return;
		}
		std::cout << " buses";
		for (auto& bus : setbus) {
			std::cout << " " << bus;
		}
		std::cout << std::endl;
	}
}//namespace Stats