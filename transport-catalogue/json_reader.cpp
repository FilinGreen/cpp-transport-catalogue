#include "json_reader.h"




JSONReader::JSONReader(const json::Node& data) :
	data_(data) {}

void JSONReader::LoadData() {
	json::Node base_data = (data_.AsMap()).at("base_requests");

	LoadStops(base_data);
	LoadBuses(base_data);
}
void JSONReader::ProcessQuery(std::ostream& out) {
	json::Node requests_data = (data_.AsMap()).at("stat_requests");

	std::vector<json::Node> result;                                                     // Контейнер обработки запросов

	for (json::Node request : requests_data.AsArray()) {                                // Проходим по всем запросам
		std::map<std::string, json::Node> answer;

		if (((request.AsMap()).at("type")).AsString() == "Bus") {
			ProcessBus(answer, request);
		}
		else if (((request.AsMap()).at("type")).AsString() == "Stop") {
			ProcessStop(answer, request);
		}
		else {
			ProcessMap(answer, request);
		}

		result.push_back(answer);                                                        // Добавляем ответ на запрос в контейнер
	}//for

	json::Print(json::Document{std::move(result)}, out);                                 // Выводим ответы в поток вывода out
}

void JSONReader::LoadStops(const json::Node& base_data) {
	for (json::Node stop_data : base_data.AsArray()) {                                                       //Пробегаемся по контейнеру заполнения базы
		auto mapa = stop_data.AsMap();
		if (((mapa.at("type")).AsString()) == "Stop") {                                                      //Проверяем что элемент является остановкой
			std::string stopname = (mapa.at("name")).AsString();                                             //Имя остановки
			catalog_.AddStop(stopname, (mapa.at("latitude")).AsDouble(), (mapa.at("longitude")).AsDouble()); //Добавляем остановку
			for (auto [name, distance] : (mapa.at("road_distances")).AsMap()) {                              //Добавляем дистанции
				catalog_.AddDistance(stopname, name, distance.AsDouble());
			}
		}
	}
}
void JSONReader::LoadBuses(const json::Node& base_data) {
	for (json::Node stop_data : base_data.AsArray()) {                 //Пробегаем по контейнеру заполнения базы
		auto mapa = stop_data.AsMap();
		if (((mapa.at("type")).AsString()) == "Bus") {                 //Проверяем что элемент является маршрутом/автобусом
			std::vector<std::string> bus_stops;
			bus_stops.reserve(((mapa.at("stops")).AsArray()).size());

			for (json::Node stop : ((mapa.at("stops")).AsArray())) {   //Заполняем контейнер остановок
				bus_stops.push_back(stop.AsString());
			}

			if (!(mapa.at("is_roundtrip")).AsBool()) {                 //Если маршрут не кольцевой то добавляем остановки в обратную сторону

				bus_stops.reserve(((mapa.at("stops")).AsArray()).size() * 2);
				size_t z = bus_stops.size() - 2;

				for (size_t i = z; i + 1 > 0; --i) {
					bus_stops.push_back(bus_stops.at(i));
				}//for

			}//if

			catalog_.AddBus((mapa.at("name")).AsString(), bus_stops, (mapa.at("is_roundtrip")).AsBool());   //Добавляем маршрут/автобус
		}
	}
}

void JSONReader::ProcessBus(std::map<std::string, json::Node>& answer, const json::Node& request) {
	std::string bus_name = ((request.AsMap()).at("name")).AsString();

	if (!catalog_.HasBus(bus_name)) {                                // Проверка наличия маршрута в каталоге
		answer["request_id"] = (request.AsMap()).at("id");
		answer["error_message"] = "not found";
		return;
	}

	double geo_distance = 0.0;                                      // Общая географическая дистанция
	int route_lenght = 0;                                           // Общая реальная дистанция
	std::unordered_set <std::string_view> unique_stops;             // Сет уникальных остановок
	Stop* prev_stop = catalog_.GetBus(bus_name).route.at(0);         // Предыдущая остановка
	bool flag = true;

	for (auto& stop : catalog_.GetBus(bus_name).route) {             // Обработка маршрута
		unique_stops.insert((*stop).name);
		if (flag) {
			flag = false;

			continue;
		}
		geo_distance += geo::ComputeDistance({ (*prev_stop).x,(*prev_stop).y }, { (*stop).x,(*stop).y });
		route_lenght += catalog_.GetDistance(prev_stop, stop);
		prev_stop = stop;

	}//for

	//Добавление данных в файл ответа
	answer["request_id"] = (request.AsMap()).at("id");
	answer["stop_count"] = static_cast<int>((catalog_.GetBus(bus_name).route).size());
	answer["curvature"] = route_lenght / geo_distance;
	answer["route_length"] = route_lenght;
	answer["unique_stop_count"] = static_cast<int>(unique_stops.size());
}
void JSONReader::ProcessStop(std::map<std::string, json::Node>& answer, const json::Node& request) {
	std::string stop_name = ((request.AsMap()).at("name")).AsString(); // Имя остановки

	if (!catalog_.HasStop(stop_name)) {                                 // Проверка что остановка существует
		answer["request_id"] = (request.AsMap()).at("id");
		answer["error_message"] = "not found";
		return;
	}

	std::set<std::string_view> buses = catalog_.GetBuses(stop_name);    // Получение списка маршрутов остановки из каталога
	std::vector<json::Node> sbuses;
	sbuses.reserve(buses.size());

	for (std::string_view bus : buses) {
		sbuses.push_back(std::string(bus));
	}

	answer["request_id"] = (request.AsMap()).at("id");                 // Добавление id запроса в файл ответа
	answer["buses"] = sbuses;                                          // Добавлениеконтейнера маршрутов в файл ответа
}

renderer::SphereProjector JSONReader::SetProjector(TransportCatalogue& catalog, renderer::RenderSettings& settings) { // Задаем класс смещения
	std::vector<geo::Coordinates> allcoor;
	for (auto& stop : catalog.GetStops()) {
		if (catalog.GetBuses(stop.name).size() != 0) {
			allcoor.push_back({ stop.x, stop.y });
		}
	}
	renderer::SphereProjector proj{ allcoor.begin(), allcoor.end(), settings.size.x, settings.size.y, settings.padding};
	return proj;
}

renderer::RenderSettings JSONReader::LoadSettings(const json::Node& data) { // Загрузка настроек отображения карты
	json::Node graphic_data = (data.AsMap()).at("render_settings");

	renderer::RenderSettings settings;
	std::map<std::string, json::Node> sets = graphic_data.AsMap();
	settings.size.x = (sets.at("width")).AsDouble();
	settings.size.y = (sets.at("height")).AsDouble();
	settings.padding = (sets.at("padding")).AsDouble();
	settings.line_width = (sets.at("line_width")).AsDouble();
	settings.stop_radius = (sets.at("stop_radius")).AsDouble();
	settings.bus_label_font_size = (sets.at("bus_label_font_size")).AsInt();
	settings.bus_label_offset = LoadOffset((sets.at("bus_label_offset")).AsArray());
	settings.stop_label_font_size = (sets.at("stop_label_font_size")).AsInt();
	settings.stop_label_offset = LoadOffset((sets.at("stop_label_offset")).AsArray());
	settings.underlayer_color = LoadColor(sets.at("underlayer_color"));
	settings.underlayer_width = (sets.at("underlayer_width")).AsDouble();

	std::vector<svg::Color> colors;
	colors.reserve(sets.at("color_palette").AsArray().size());
	for (json::Node color : sets.at("color_palette").AsArray()) {
		colors.push_back(LoadColor(color));
	}
	settings.color_palette = colors;

	return settings;
}

svg::Color JSONReader::LoadColor(const json::Node& data) { // Загрузка Color из json
	if (data.IsString()) {
		return data.AsString();
	}
	else if (data.IsArray()) {
		if ((data.AsArray()).size() == 3) {
			return svg::Rgb(
				(data.AsArray()).at(0).AsInt(),
				(data.AsArray()).at(1).AsInt(),
				(data.AsArray()).at(2).AsInt());
		}
		else {
			return svg::Rgba(
				(data.AsArray()).at(0).AsInt(),
				(data.AsArray()).at(1).AsInt(),
				(data.AsArray()).at(2).AsInt(),
				(data.AsArray()).at(3).AsDouble());
		}
	}
	return svg::NoneColor;
}

svg::Point JSONReader::LoadOffset(const std::vector<json::Node>& data) { //Загрузка Point из json
	svg::Point result;
	if (data.size() > 1) {
		result.x = data.at(0).AsDouble();
		result.y = data.at(1).AsDouble();
	}
	return result;
}

void JSONReader::ProcessMap(std::map<std::string, json::Node>& answer, const json::Node& request) {
	
	renderer::RenderSettings settings = LoadSettings(data_);
	renderer::SphereProjector proj = SetProjector(catalog_,settings);

	std::ostringstream sout;                                  // Определение потока вывода
	MapRenderer maprender(data_);
	maprender.Draw(catalog_, settings, sout, proj);                           // Отрисовка карты и вывод svg файла в поток вывода          

	answer["map"] = sout.str();                               // Добавление карты в файл ответа
	answer["request_id"] = (request.AsMap()).at("id");        // Добавление id запроса в файл ответа
}


