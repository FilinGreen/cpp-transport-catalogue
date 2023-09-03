#include "transport_router.h"

TransportRouter::TransportRouter(TransportCatalogue& catalog, int bt, double bv) :catalog_(catalog), bus_wait_time_(bt), bus_velocity_(bv) {
	graph_ = std::make_unique <graph::DirectedWeightedGraph<double>>(catalog.GetStopsCount() * 2);//Создаем граф с количеством вершин по 2 на остановку
	FillGraph();
	router_ = std::make_unique<graph::Router <double>>(*graph_);
}


void TransportRouter::FillGraph() {

	for (Stop& stop : catalog_.GetStops()) {//Проходим по всем остановкам
		graph::Edge<double> edge = { stop.stop_id, stop.stop_id + 1, bus_wait_time_ };

		edge_info_[graph_->AddEdge(edge)] = std::make_unique < edgeinfo >(stop.name, 0, stop.stop_id); //Добавляем ребро в граф и информацию в словарь (name, span_count, stop_id)
	}

	for (Bus& bus : catalog_.GetBuses()) {//проходим по всем маршрутам
		if (bus.circle) {
			AddCircleBusEdges(bus);
		}
		else {
			AddLineBusEdges(bus);
		}
	}
}


void TransportRouter::AddBusEdges(Bus& bus, std::vector<Stop* >::iterator begin, std::vector<Stop* >::iterator end) {
	for (auto stop1 = begin; stop1 != end - 1; ++stop1) {                      //проходим по всем остановкам маршрута

		double velocity = bus.bus_velocity;
		double all_distance_time = 0;                                        //дистанция от текущей остановки до stop2 без пересадок
		std::vector<Stop* >::iterator prev_stop = stop1;

		bool first_distance = true;
		for (auto stop2 = stop1 + 1; stop2 != end; ++stop2) {                //проходим по остановкам того же маршрута чтобы отрисовать ребра
			double distance = catalog_.GetDistanceEdge(*stop1, *stop2);

			if (!first_distance || distance == 0) {                                            //Если в маршруте есть прямой путь от stop1 к stop2
				distance = catalog_.GetDistance(*prev_stop, *stop2);
				if (distance == 0) {
					std::cout << "Это сообщение не выведется т.к. данные обещали корректные";
					break;
					//continue;
				}
			}


			double ride_time = distance / velocity;
			all_distance_time += ride_time;
			graph::Edge<double> edge = { (*stop1)->stop_id + 1, (*stop2)->stop_id, all_distance_time };
			edge_info_[graph_->AddEdge(edge)] = std::make_unique < edgeinfo >(bus.name, std::distance(stop1, stop2), (*stop2)->stop_id);
			prev_stop = stop2;
			first_distance = false;
		}
	}
}


void TransportRouter::AddCircleBusEdges(Bus& bus) {
	AddBusEdges(bus, bus.route.begin(), bus.route.end());
}


void TransportRouter::AddLineBusEdges(Bus& bus) {
	size_t mid_num = (bus.route.size() - 1) / 2;                        //находим индекс среднего элемента 
	std::vector<Stop* >::iterator mid = bus.route.begin() + mid_num;  //итератор среднего элемента

	AddBusEdges(bus, bus.route.begin(), mid + 1);
	AddBusEdges(bus, mid, bus.route.end());
}


void TransportRouter::ProcessRoute(json::Builder& builder, const json::Node& request, size_t from, size_t to) {
	std::optional<graph::Router<double>::RouteInfo> info = router_->BuildRoute(from, to);

	if (!info.has_value()) {//Если нет маршрута вывести сообщение

		builder.StartDict();
		builder.Key("request_id").Value((request.AsDict()).at("id").AsInt());     // Добавление id запроса в файл ответа
		builder.Key("error_message").Value(std::string(std::string("not found")));
		builder.EndDict();
	}
	else {
		builder.StartDict();
		double time = info.value().weight;                                        //Общее время 

		builder.Key("request_id").Value((request.AsDict()).at("id").AsInt());     // Добавление id запроса в файл ответа
		builder.Key("total_time").Value(time);                                    // Добавления времени которое требуется для прохождения маршрута



		builder.Key("items").StartArray();                                        // Добавление словаря items


		bool wait = true;

		for (auto edgeid : info.value().edges) {                                  //Пробегаем по вектору EdgeId
			auto& edge = edge_info_.at(edgeid);                                   //Получаем Edge по его id 
			if (!wait) {
				wait = true;

				builder.StartDict();
				builder.Key("type").Value(std::string("Bus"));
				builder.Key("bus").Value(edge->name);                             //имя маршрута 
				builder.Key("span_count").Value(edge->stop_count);                //количество остановок
				builder.Key("time").Value(graph_->GetEdge(edgeid).weight);        //время маршрута 
				builder.EndDict();
			}
			else {
				wait = false;
				std::string_view stopname = catalog_.GetStopNameById(edge_info_.at(edgeid)->stopid);

				builder.StartDict();
				builder.Key("type").Value(std::string("Wait"));
				builder.Key("stop_name").Value(std::string(stopname));
				builder.Key("time").Value(bus_wait_time_);
				builder.EndDict();
			}

		}

		builder.EndArray();//items

		builder.EndDict();//query
	}
}