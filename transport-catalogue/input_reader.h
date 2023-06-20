#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "transport_catalogue.h"

void LoadTC(TransportCatalogue& result, std::istream& input);//Необходимо реализовать буфер (склад запросов) либо их сортировка. Как вариант 2 vector

std::vector<std::string> SplitInToWordsStop(std::string& text);

std::vector<std::string> SplitIntoWordsBus(std::string& text, char lit);

void AddStop (TransportCatalogue& catalog, std::string& text);

void AddBus (TransportCatalogue& catalog, std::string& text);