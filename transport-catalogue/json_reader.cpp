#include "json_reader.h"

namespace Input {
	void LoadTC(TransportCatalogue& result, std::istream& input) {
		int additions = 0;
		input >> additions;
		std::string text;
		std::getline(input, text);
		std::vector<std::string> stops;
		std::vector<std::string> buses;
		for (int i = 0; i < additions; ++i) {
			std::getline(input, text);
			if (text[0] == 'S') {
				stops.push_back(std::move(text));
			}
			else {
				buses.push_back(std::move(text));
			}
		}
		for (auto& stop : stops) {
			AddStop(result, stop);
		}
		for (auto& bus : buses) {
			AddBus(result, bus);
		}




	}

	std::vector<std::string> SplitInToWordsStop(std::string& text) {
		std::vector<std::string> result;
		size_t begin = text.find(' ') + 1;
		size_t end = text.find(':');
		result.push_back(text.substr(begin, end - begin));     //имя
		begin = text.find_first_not_of(' ', end + 1);
		end = text.find(' ', begin) - 1;
		result.push_back(text.substr(begin, end - begin));     //x
		begin = text.find_first_not_of(' ', end + 2);
		end = text.find(' ', begin) - 1;
		result.push_back(text.substr(begin, end - begin));     //y
		begin = text.find_first_not_of(' ', end + 2);
		while (begin != 0) {
			end = text.find(' ', begin);                       //Конец DNm
			result.push_back(text.substr(begin, end - begin - 1));
			begin = end + 4;
			end = text.find(',', begin);
			result.push_back(text.substr(begin, end - begin)); //Название остановки
			begin = text.find_first_not_of(' ', end + 1);
		}
		return result;
	}


	std::vector<std::string> SplitIntoWordsBus(std::string& text, char lit) {
		std::vector<std::string> result;
		size_t begin = text.find(' ') + 1;                  //Нашли букву начала имени автобуса
		size_t end = text.find(':');                        //Конец имени автобуса
		result.push_back(text.substr(begin, end - begin));  // добавляем имя автобуса

		begin = text.find_first_not_of(' ', end + 1);       //Первая буква названия остановки
		while (begin != 0) {
			end = text.find(lit, begin);
			result.push_back(text.substr(begin, end - 1 - begin));
			begin = text.find_first_not_of(' ', end + 1);
		}
		return result;
	}

	void AddStop(TransportCatalogue& catalog, std::string& text) { //добавление остановки и дистанций
		std::vector<std::string> vec = SplitInToWordsStop(text);                
		std::string stopname = vec.at(0);                                       
		catalog.AddStop(stopname, std::stod(vec.at(1)), std::stod(vec.at(2)));

		for (size_t i = 3; i < vec.size(); ) {
			catalog.AddDistance(stopname, vec.at(i + 1), std::stod(vec.at(i)));
			i += 2;
		}
	}

	void AddBus(TransportCatalogue& catalog, std::string& text) { //Добавление маршрута
		std::vector<std::string> bus_stops;
		std::vector<std::string> vec;
		if (text.find('>') != std::string::npos) {
			vec = SplitIntoWordsBus(text, { '>' });
			for (size_t i = 1; i < vec.size(); ++i) {
				bus_stops.push_back(vec.at(i));
			}
		}
		else {
			vec = SplitIntoWordsBus(text, { '-' });
			for (size_t i = 1; i < vec.size(); ++i) {
				bus_stops.push_back(vec.at(i));
			}
			for (size_t i = vec.size() - 2; i > 0; --i) {
				bus_stops.push_back(vec.at(i));
			}
		}
		catalog.AddBus(vec.at(0), bus_stops);
	}
}//namespace Input 

//-----------------------------------------Stats-----------------------------------------------------------------------

namespace Stats {
	void ExecuteQueries(TransportCatalogue& catalog, std::istream& input, std::ostream& out) { //обработка запросов
		int queries_number = 0;
		input >> queries_number;
		std::string text;
		std::getline(input, text);

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
		std::string name = text.substr(text.find(' ') + 1);                       // Имя маршрута
		if (!catalog.HasBus(name)) {
			out << "Bus " << name << ": not found" << std::endl;
			return;
		}

		double distance = 0;                                                      // Общая географическая дистанция
		double real_distance = 0;                                                 // Общая реальная длина
		std::unordered_set <std::string_view> unique_stops;                       // Сет уникальных остановок
		Stop* prev_stop = catalog.GetBus(name).route.at(0);                       // Предыдущая остановка
		bool flag = true;
		for (auto& stop : catalog.GetBus(name).route) {                           // Обработка маршрута
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


	void ProcessStop(TransportCatalogue& catalog, std::string& text, std::ostream& out) { // Обработка запроса становки
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

namespace Reader {
	
	//------------------------------------------------------------Load Base----------------------------------------------------------------------
	void LoadStops(TransportCatalogue& catalog, json::Node base_data) {                                         //Загрузка данных о остановках из base_data

		for (json::Node stop_data : base_data.AsArray()) {                                                      //Пробегаемся по контейнеру заполнения базы
			auto mapa = stop_data.AsMap();
			if (((mapa.at("type")).AsString()) == "Stop") {                                                     //Проверяем что элемент является остановкой
				std::string stopname = (mapa.at("name")).AsString();                                            //Имя остановки
				catalog.AddStop(stopname, (mapa.at("latitude")).AsDouble(), (mapa.at("longitude")).AsDouble()); //Добавляем остановку
				for (auto [name, distance] : (mapa.at("road_distances")).AsMap()) {                             //Добавляем дистанции
					catalog.AddDistance(stopname, name, distance.AsDouble());
				}
			}
		}
	}

	void LoadBuses(TransportCatalogue& catalog, json::Node base_data) {    //Загрузка данных о маршрутах из base_data

		for (json::Node stop_data : base_data.AsArray()) {                 //Пробегаем по контейнеру заполнения базы
			auto mapa = stop_data.AsMap();
			if (((mapa.at("type")).AsString()) == "Bus") {                 //Проверяем что элемент является маршрутом/автобусом
				std::vector<std::string> bus_stops;
				bus_stops.reserve(((mapa.at("stops")).AsArray()).size());

				for (json::Node stop : ((mapa.at("stops")).AsArray())) {   //Заполняем контейнер остановок
					bus_stops.push_back(stop.AsString());
				}

				if (!(mapa.at("is_roundtrip")).AsBool()) {                 //Если маршрут не кольцевой то добавляем остановки в обратную сторону

					bus_stops.reserve(((mapa.at("stops")).AsArray()).size()*2);
					size_t z = bus_stops.size()-2;

					for (size_t i = z; i+1 > 0; --i) {
						bus_stops.push_back(bus_stops.at(i));
					}//for

				}//if

				catalog.AddBus((mapa.at("name")).AsString(), bus_stops, (mapa.at("is_roundtrip")).AsBool());   //Добавляем маршрут/автобус
			}
		}
	}

	void LoadData(TransportCatalogue& catalog, json::Node data) { //Загрузка данных из data
		json::Node base_data = (data.AsMap()).at("base_requests");

		LoadStops(catalog, base_data);
		LoadBuses(catalog, base_data);
	}


	//------------------------------------------------------------Query processing----------------------------------------------------------------------
	
	void BusProcessing(TransportCatalogue& catalog, std::map<std::string, json::Node>& answer, json::Node request) {
		std::string bus_name = ((request.AsMap()).at("name")).AsString();

		if (!catalog.HasBus(bus_name)) {                                // Проверка наличия маршрута в каталоге
			answer["request_id"] = (request.AsMap()).at("id");
			answer["error_message"] = "not found";
			return;
		}

		double geo_distance = 0.0;                                      // Общая географическая дистанция
		int route_lenght = 0;                                           // Общая реальная дистанция
		std::unordered_set <std::string_view> unique_stops;             // Сет уникальных остановок
		Stop* prev_stop = catalog.GetBus(bus_name).route.at(0);         // Предыдущая остановка
		bool flag = true;

		for (auto& stop : catalog.GetBus(bus_name).route) {             // Обработка маршрута
			unique_stops.insert((*stop).name);
			if (flag) {
				flag = false;

				continue;
			}
			geo_distance += geo::ComputeDistance({ (*prev_stop).x,(*prev_stop).y }, { (*stop).x,(*stop).y });
			route_lenght += catalog.GetDistance(prev_stop, stop);
			prev_stop = stop;

		}//for

		//Добавление данных в файл ответа
		answer["request_id"] = (request.AsMap()).at("id");
		answer["stop_count"] = static_cast<int>((catalog.GetBus(bus_name).route).size());
		answer["curvature"] = route_lenght / geo_distance;
		answer["route_length"] = route_lenght;
		answer["unique_stop_count"] = static_cast<int>(unique_stops.size());
	}

	void StopProcessing(TransportCatalogue& catalog, std::map<std::string, json::Node>& answer, json::Node request) {

		std::string stop_name = ((request.AsMap()).at("name")).AsString(); // Имя остановки

		if (!catalog.HasStop(stop_name)) {                                 // Проверка что остановка существует
			answer["request_id"] = (request.AsMap()).at("id");
			answer["error_message"] = "not found";
			return;
		}
		
		std::set<std::string_view> buses = catalog.GetBuses(stop_name);    // Получение списка маршрутов остановки из каталога
		std::vector<json::Node> sbuses;
		sbuses.reserve(buses.size());

		for (std::string_view bus : buses) {
			sbuses.push_back(std::string(bus));
		}

		answer["request_id"] = (request.AsMap()).at("id");                 // Добавление id запроса в файл ответа
		answer["buses"] = sbuses;                                          // Добавлениеконтейнера маршрутов в файл ответа
	}

	void MapProcessing(TransportCatalogue& catalog, std::map<std::string, json::Node>& answer, json::Node request, json::Node data) {
		std::ostringstream sout;                                  // Определение потока вывода 
		GraphicProcessing(catalog, data, sout);                   // Отрисовка карты и вывод svg файла в поток вывода
		answer["map"] = sout.str();                               // Добавление карты в файл ответа
        answer["request_id"]= (request.AsMap()).at("id");         // Добавление id запроса в файл ответа
	}

	void QueryProcessing(TransportCatalogue& catalog, json::Node data, std::ostream& out) { // Обработка запросов к транспортному каталогу
		json::Node requests_data = (data.AsMap()).at("stat_requests");

		std::vector<json::Node> result;                                                     // Контейнер обработки запросов

		for (json::Node request : requests_data.AsArray()) {                                // Проходим по всем запросам
			std::map<std::string, json::Node> answer;

			if (((request.AsMap()).at("type")).AsString() == "Bus") {                       
				BusProcessing(catalog, answer, request);
			}
			else if (((request.AsMap()).at("type")).AsString() == "Stop") {
				StopProcessing(catalog, answer, request);
			}
			else {
				MapProcessing(catalog, answer, request, data);
			}

			result.push_back(answer);                                                        // Добавляем ответ на запрос в контейнер 
		}//for

		json::Print(json::Document{std::move(result)}, out);                                 // Выводим ответы в поток вывода out
	}
	
	//------------------------------------------------------------Graphic processing----------------------------------------------------------------------

	svg::Point LoadOffset(std::vector<json::Node> data) { //Загрузка Point из json
		svg::Point result;
		if (data.size() > 1) {
			result.x = data.at(0).AsDouble();
			result.y = data.at(1).AsDouble();
		}
		return result;
	}

	svg::Color LoadColor(json::Node data) { // Загрузка Color из json
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

	renderer::Render_settings LoadSettings(json::Node data) { // Загрузка настроек отображения карты
		json::Node graphic_data = (data.AsMap()).at("render_settings");

		renderer::Render_settings settings;
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

	void DrowLine(svg::Document& doc, renderer::Render_settings& settings, renderer::SphereProjector& proj, int color, std::vector<Stop*>& route) {

					svg::Polyline line;                                //создаем линию и заполняем данные
					line.SetFillColor("none");
					line.SetStrokeWidth(settings.line_width);
					line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
					line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
					line.SetStrokeColor(settings.color_palette.at(color));

					std::vector<geo::Coordinates> coordinates;         //вектор координат остановок маршрута
					for (size_t i = 0; i < route.size(); ++i) {
						coordinates.push_back({route.at(i)->x, route.at(i)->y});
					}
			    
					for (auto& point : coordinates) {                   //проходим по вектору и проецируем все координаты добавляя их в линию
						svg::Point p = proj(point);
						line.AddPoint({p.x, p.y});
					}

					doc.Add(line);                                       //добавляем примитив в документ примитивов
	}

	renderer::SphereProjector SetProjector(TransportCatalogue& catalog, renderer::Render_settings& settings) { // Задаем класс смещения
		std::vector<geo::Coordinates> allcoor;
		for (auto& stop : catalog.GetStops()) {
			if (catalog.GetBuses(stop.name).size() != 0) {
				allcoor.push_back({ stop.x, stop.y });
			}
		}
	    renderer::SphereProjector proj{ allcoor.begin(), allcoor.end(), settings.size.x, settings.size.y, settings.padding};
		return proj;
	}

	void DrowStops(svg::Document& doc, geo::Coordinates coordinates, renderer::SphereProjector& proj, renderer::Render_settings& settings) { // Отрисовка графического отображения остановки

			svg::Circle circle;
			svg::Point point = proj(coordinates);
			circle.SetCenter(point);
			circle.SetRadius(settings.stop_radius);
			circle.SetFillColor("white");
			doc.Add(circle);

	}

	void DrowBusname(TransportCatalogue& catalog, std::string busname, svg::Document& doc, renderer::Render_settings& settings, renderer::SphereProjector& proj, int color) { // Отрисовка названий маршрута
			Bus bus = catalog.GetBus(busname);

			geo::Coordinates bus_coor = { bus.route.at(0)->x , bus.route.at(0)->y };
			svg::Point point = proj(bus_coor);
			svg::Text under;
			svg::Text upper;
			under.SetPosition({ point.x, point.y });
			upper.SetPosition({ point.x, point.y });
			under.SetOffset(settings.bus_label_offset);
			upper.SetOffset(settings.bus_label_offset);
			under.SetFontSize(settings.bus_label_font_size);
			upper.SetFontSize(settings.bus_label_font_size);
			under.SetFontFamily("Verdana");
			upper.SetFontFamily("Verdana");
			under.SetFontWeight("bold");
			upper.SetFontWeight("bold");
			under.SetData(busname);
			upper.SetData(busname);
			under.SetFillColor(settings.underlayer_color);
			under.SetStrokeColor(settings.underlayer_color);
			upper.SetFillColor(settings.color_palette.at(color));
			under.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			under.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			under.SetStrokeWidth(settings.underlayer_width);

			doc.Add(under);
			doc.Add(upper);

			if (!bus.circle&& bus.route.at(0) != bus.route.at((bus.route.size() - 1) / 2)) { //Проверка что маршрут не кольцевой и конечные остановки не совпадают

				size_t i = (bus.route.size() - 1) / 2;

				geo::Coordinates bus_coor2 = { bus.route.at(i)->x , bus.route.at(i)->y };
				svg::Point point2 = proj(bus_coor2);
				svg::Text under2;
				svg::Text upper2;
				under2.SetPosition({ point2.x, point2.y });
				upper2.SetPosition({ point2.x, point2.y });
				under2.SetOffset(settings.bus_label_offset);
				upper2.SetOffset(settings.bus_label_offset);
				under2.SetFontSize(settings.bus_label_font_size);
				upper2.SetFontSize(settings.bus_label_font_size);
				under2.SetFontFamily("Verdana");
				upper2.SetFontFamily("Verdana");
				under2.SetFontWeight("bold");
				upper2.SetFontWeight("bold");
				under2.SetData(busname);
				upper2.SetData(busname);
				under2.SetFillColor(settings.underlayer_color);
				under2.SetStrokeColor(settings.underlayer_color);
				upper2.SetFillColor(settings.color_palette.at(color));
				under2.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				under2.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
				under2.SetStrokeWidth(settings.underlayer_width);
				doc.Add(under2);
				doc.Add(upper2);
			}
	}

	void DrowStopName(svg::Document& doc, geo::Coordinates coordinates, renderer::Render_settings& settings, renderer::SphereProjector& proj, std::string stopname) { // Отрисовка названия остановки
			
			svg::Point point = proj(coordinates);
			svg::Text under;
			svg::Text upper;
			under.SetPosition({ point.x, point.y });
			upper.SetPosition({ point.x, point.y });
			under.SetOffset(settings.stop_label_offset);
			upper.SetOffset(settings.stop_label_offset);
			under.SetFontSize(settings.stop_label_font_size);
			upper.SetFontSize(settings.stop_label_font_size);
			under.SetFontFamily("Verdana");
			upper.SetFontFamily("Verdana");
			under.SetData(stopname);
			upper.SetData(stopname);
			under.SetFillColor(settings.underlayer_color);
			under.SetStrokeColor(settings.underlayer_color);
			upper.SetFillColor("black");
			under.SetStrokeWidth(settings.underlayer_width);
			under.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			under.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

			doc.Add(under);
			doc.Add(upper);

	}

	void Drow(TransportCatalogue& catalog, renderer::Render_settings& settings, std::ostream& out, renderer::SphereProjector& proj) {
		svg::Document doc;                                             //Документ для печати примитивов
		
		std::vector<std::string> buses;                                //вектор маршрутов чтоб потом сортировать
		buses.reserve((catalog.GetBuses()).size());

		for (Bus& bus : catalog.GetBuses()) {                          //заполняем вектор маршрутов
			if (bus.route.size() != 0) {
				buses.push_back(bus.name);
			}
		}
		std::sort(buses.begin(), buses.end());                         //сортируем вектор маршрутов


		int max_color = settings.color_palette.size();                 //количество цветов
		int bus_number = max_color;                                    //начальная переменная для цвета
		                                                                
		for (std::string& busname : buses) {                           //проходим по маршрутам и отрисовка линий для маршрутов с остановками
				DrowLine(doc, settings, proj, bus_number % max_color, catalog.GetBus(busname).route);
				++bus_number;
		}

	    bus_number = max_color;                                        //начальная переменная для цвета

		for (std::string& busname : buses) {                           //проходим по маршрутам и отрисовка названий для маршрутов с остановками
				DrowBusname(catalog, busname, doc, settings, proj, bus_number % max_color);
				++bus_number;
		}

		std::vector<std::string> stops;                                 //вектор остановок чтобы потом сортировать
		stops.reserve((catalog.GetStops()).size());

		for (Stop& stop : catalog.GetStops()) {
			if (catalog.HasBusAtStop(stop.name)) {
				stops.push_back(stop.name);
			}
		}

		std::sort(stops.begin(), stops.end());

		for (std::string& stopname : stops) {                              //отрисовка остановок
			DrowStops(doc,{ (catalog.GetStop(stopname)).x,(catalog.GetStop(stopname)).y}, proj, settings);
		}

		for (std::string& stopname : stops) {                              //отрисовка названий остановок
			DrowStopName(doc, { (catalog.GetStop(stopname)).x,(catalog.GetStop(stopname)).y }, settings, proj, stopname);
		}

		doc.Render(out);

	}

	void GraphicProcessing(TransportCatalogue& catalog, json::Node data, std::ostringstream& out) { //Отрисовка карты
		renderer::Render_settings settings = LoadSettings(data);              // Загрузка настроек отображения
		renderer::SphereProjector proj = SetProjector(catalog, settings);     // Настройка класса смещения
		Drow(catalog, settings, out, proj);                                   // Отрисовка карты и вывод в поток out
	}

}//namespace Reader