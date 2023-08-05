#include "stat_reader.h"
#include "domain.h"

namespace Stats {
	void ExecuteQueries(TransportCatalogue& catalog, std::istream& input, std::ostream& out) {
		int queries_number = 0;
		input >> queries_number;
		std::string text;
		std::getline(input, text);//По какой то причине после того как считываю гетлайном количество команд, следующий гетлайн идет пустой

		for (int i = 0; i < queries_number; ++i) {
			std::getline(input, text);
			if (text.at(0) == 'S') {
				 ProcessStop(catalog, text, out);
			}
			else {
				ProcessBus(catalog, text, out);
			}

		}
	}

	void  ProcessBus(TransportCatalogue& catalog, std::string& text, std::ostream& out) {

		std::string name = text.substr(text.find(' ') + 1);                       // Именя маршрута
		if (!catalog.HasBus(name)) {
			out << "Bus " << name << ": not found" << std::endl;
			return;
		}

		double distance = 0;                                                      // Общая географическая дистанция
		double real_distance = 0;                                                 // Общая реальная длина
		std::unordered_set <std::string_view> unique_stops;                       // Сет уникальных остановок
		Stop* prev_stop = catalog.GetBus(name).route.at(0);    // Предыдущая остановка
		bool flag = true;
		for (auto& stop : catalog.GetBus(name).route) {
			unique_stops.insert((*stop).name);
			if (flag) {
				flag = false;

				continue;
			}
			distance += geo::ComputeDistance({ (*prev_stop).x,(*prev_stop).y }, { (*stop).x,(*stop).y });
			real_distance += catalog.GetDistance(prev_stop, stop);
			prev_stop = stop;

		}

		out << "Bus " << name << ": " << catalog.GetBus(name).route.size() << " stops on route, " << unique_stops.size() << " unique stops, " << real_distance << " route length, " << real_distance / distance << " curvature" << std::endl;

	}


	void ProcessStop(TransportCatalogue& catalog, std::string& text, std::ostream& out) {
		std::string name = text.substr(text.find(' ') + 1);
		out << "Stop " << name << ":";

		if (!catalog.HasStop(name)) {
			out << " not found" << std::endl;
			return;
		}
		std::set<std::string_view> setbus = catalog.GetBuses(name);
		if (setbus.size() == 0) {
			out << " no buses" << std::endl;
			return;
		}
		out << " buses";
		for (auto& bus : setbus) {
			std::cout << " " << bus;
		}
		out << std::endl;
	}
}//namespace Stats