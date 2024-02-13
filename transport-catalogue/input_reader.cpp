#include "input_reader.h"

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
		result.push_back(text.substr(begin, end - begin));//имя
		begin = text.find_first_not_of(' ', end + 1);
		end = text.find(' ', begin) - 1;
		result.push_back(text.substr(begin, end - begin));//x
		begin = text.find_first_not_of(' ', end + 2);
		end = text.find(' ', begin) - 1;
		result.push_back(text.substr(begin, end - begin));//y
		begin = text.find_first_not_of(' ', end + 2);
		while (begin != 0) {
			end = text.find(' ', begin);//Конец DNm
			result.push_back(text.substr(begin, end - begin - 1));
			begin = end + 4;
			end = text.find(',', begin);
			result.push_back(text.substr(begin, end - begin));//Название остановки
			begin = text.find_first_not_of(' ', end + 1);
		}
		return result;
	}


	std::vector<std::string> SplitIntoWordsBus(std::string& text, char lit) {
		std::vector<std::string> result;
		size_t begin = text.find(' ') + 1;//Нашли букву начала имени автобуса
		size_t end = text.find(':');//Конец имени автобуса
		result.push_back(text.substr(begin, end - begin));// добавляем имя автобуса

		begin = text.find_first_not_of(' ', end + 1);//Первая буква названия остановки
		while (begin != 0) {
			end = text.find(lit, begin);
			result.push_back(text.substr(begin, end - 1 - begin));
			begin = text.find_first_not_of(' ', end + 1);

		}
		return result;
	}

	void AddStop(TransportCatalogue& catalog, std::string& text) {
		std::vector<std::string> vec = SplitInToWordsStop(text);
		std::string stopname = vec.at(0);
		catalog.AddStop(vec.at(0), std::stod(vec.at(1)), std::stod(vec.at(2)));

		for (size_t i = 3; i < vec.size(); ) {
			catalog.AddDistance(stopname, vec.at(i + 1), std::stod(vec.at(i)));
			i += 2;
		}



	}

	void AddBus(TransportCatalogue& catalog, std::string& text) {
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
