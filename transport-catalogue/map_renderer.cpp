#include "map_renderer.h"

namespace rendermap {

	svg::Point LoadOffset(const std::vector<json::Node>& data) { //Загрузка Point из json
		svg::Point result;
		if (data.size() > 1) {
			result.x = data.at(0).AsDouble();
			result.y = data.at(1).AsDouble();
		}
		return result;
	}

	svg::Color LoadColor(const json::Node& data) { // Загрузка Color из json
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


	Rendersettings LoadSettings(const json::Node& data) { // Загрузка настроек отображения карты
		json::Node graphic_data = (data.AsMap()).at("render_settings");

		renderer::Rendersettings settings;
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

	SphereProjector SetProjector(TransportCatalogue& catalog, renderer::Rendersettings& settings) { // Задаем класс смещения
		std::vector<geo::Coordinates> allcoor;
		for (auto& stop : catalog.GetStops()) {
			if (catalog.GetBuses(stop.name).size() != 0) {
				allcoor.push_back({ stop.x, stop.y });
			}
		}
		renderer::SphereProjector proj{ allcoor.begin(), allcoor.end(), settings.size.x, settings.size.y, settings.padding};
		return proj;
	}


	void DrawLine(svg::Document& doc, renderer::Rendersettings& settings, renderer::SphereProjector& proj, int color, const std::vector<Stop*>& route) {

		svg::Polyline line;                                //создаем линию и заполняем данные
		line.SetFillColor("none");
		line.SetStrokeWidth(settings.line_width);
		line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
		line.SetStrokeColor(settings.color_palette.at(color));

		std::vector<geo::Coordinates> coordinates;         //вектор координат остановок маршрута
		for (size_t i = 0; i < route.size(); ++i) {
			coordinates.push_back({ route.at(i)->x, route.at(i)->y });
		}

		for (auto& point : coordinates) {                   //проходим по вектору и проецируем все координаты добавляя их в линию
			svg::Point p = proj(point);
			line.AddPoint({ p.x, p.y });
		}

		doc.Add(line);                                       //добавляем примитив в документ примитивов
	}

	void DrawBusname(TransportCatalogue& catalog, const std::string& busname, svg::Document& doc, renderer::Rendersettings& settings, renderer::SphereProjector& proj, int color) { // Отрисовка названий маршрута
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

		if (!bus.circle && bus.route.at(0) != bus.route.at((bus.route.size() - 1) / 2)) { //Проверка что маршрут не кольцевой и конечные остановки не совпадают

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

	void DrawStops(svg::Document& doc, const geo::Coordinates& coordinates, renderer::SphereProjector& proj, renderer::Rendersettings& settings) { // Отрисовка графического отображения остановки

		svg::Circle circle;
		svg::Point point = proj(coordinates);
		circle.SetCenter(point);
		circle.SetRadius(settings.stop_radius);
		circle.SetFillColor("white");
		doc.Add(circle);

	}

	void DrawStopName(svg::Document& doc, const geo::Coordinates& coordinates, renderer::Rendersettings& settings, renderer::SphereProjector& proj, const std::string& stopname) { // Отрисовка названия остановки

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

	void Draw(TransportCatalogue& catalog, renderer::Rendersettings& settings, std::ostream& out, renderer::SphereProjector& proj) {
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
			DrawLine(doc, settings, proj, bus_number % max_color, catalog.GetBus(busname).route);
			++bus_number;
		}

		bus_number = max_color;                                        //начальная переменная для цвета

		for (std::string& busname : buses) {                           //проходим по маршрутам и отрисовка названий для маршрутов с остановками
			DrawBusname(catalog, busname, doc, settings, proj, bus_number % max_color);
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
			DrawStops(doc, { (catalog.GetStop(stopname)).x,(catalog.GetStop(stopname)).y }, proj, settings);
		}

		for (std::string& stopname : stops) {                              //отрисовка названий остановок
			DrawStopName(doc, { (catalog.GetStop(stopname)).x,(catalog.GetStop(stopname)).y }, settings, proj, stopname);
		}

		doc.Render(out);

	}


	void ProcessGraphic(TransportCatalogue& catalog, const json::Node& data, std::ostringstream& out) { //Отрисовка карты
		Rendersettings settings = LoadSettings(data);                                                   // Загрузка настроек отображения
		SphereProjector proj = SetProjector(catalog, settings);                                         // Настройка класса смещения
		Draw(catalog, settings, out, proj);                                                             // Отрисовка карты и вывод в поток out
	}

}//rendermap