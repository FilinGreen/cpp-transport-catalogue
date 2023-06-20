#pragma once

#include <iostream>
#include <unordered_set>
#include <algorithm>

#include "geo.h"
#include "transport_catalogue.h"

void Query_processing (TransportCatalogue& catalog, std::istream& input);

void Single_query_processingBus(TransportCatalogue& catalog, std::string& text);

void Single_query_processingStop(TransportCatalogue& catalog, std::string& text);

