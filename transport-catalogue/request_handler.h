/*
void JSONReader::ProcessRoute(json::Builder& builder, const json::Node& request, const graph::DirectedWeightedGraph< double >& graph) {

	size_t from = catalog_.GetStop(((request.AsDict()).at("from")).AsString()).stop_id;
	size_t to = catalog_.GetStop(((request.AsDict()).at("to")).AsString()).stop_id + 1;

	graph::Router <double> router(graph);
	std::optional<graph::Router<double>::RouteInfo> info = router.BuildRoute(from, to);

	if (info.has_value()) {
		builder.StartDict();//<------------------------------------------------------
		double time = info.value().weight; //<-----------------------------------------------------------Тут исключение

		builder.Key("request_id").Value((request.AsDict()).at("id").AsInt());     // Добавление id запроса в файл ответа
		builder.Key("total_time").Value(time);                     // Добавления времени которое требуется для прохождения маршрута

		builder.Key("items").StartArray();                                         // Добавление словаря items


		int span_count = 0;
		double ride_time = 0;
		std::string prev_name = (edge_info_.at(*info.value().edges.begin()))->name;


		for (auto edgeid : info.value().edges) {
			auto& edge = edge_info_.at(edgeid);
			if (edge->name != prev_name) {
				++span_count;
				prev_name == edge->name;
				ride_time += edge->time;
			}
			else {
				if (edgeid != *(info.value().edges.begin())) {
					builder.StartDict();
					builder.Key("type").Value("Bus");
					builder.Key("bus").Value("prev_name");
					builder.Key("span_count").Value(span_count);
					builder.Key("time").Value(ride_time);//< некорректно
					builder.EndDict();
					/*
					Cоздать node map
					 {
					 "type": "Bus",
					 "bus": "297",
					 "span_count": 2,
					 "time": 5.235
					 }
					 
				}


				if (edgeid != *(info.value().edges.end() - 1)) {//<-------------------Плохая проверка

					size_t stopid = graph.GetEdge(edgeid).from;
					std::string_view stopname = catalog_.GetStopNameById(stopid);

					builder.StartDict();
					builder.Key("type").Value(std::string("Wait"));
					builder.Key("stop_name").Value(std::string(stopname));
					builder.Key("time").Value(6);
					builder.EndDict();
					
					Создать node map {
					"type": "Wait",
					"stop_name" : "Biryulyovo",
					"time" : 6
					}
					
				}

				ride_time = 0;
				span_count = 0;
			}
		}

		builder.EndArray();

		builder.EndDict();//<------------------------------------------------------
	}
	else {
		std::cout << "Throw here something" << std::endl;
	}


}
*/