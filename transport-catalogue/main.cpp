#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string_view>
#include <chrono>
#include <cassert>
#include <cstdio>

#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace json;
using namespace std::literals;


void Reader_test() {
#include "json_reader.h"

    std::ifstream ifile("itest.txt");
    if (ifile.is_open()) {
        std::cout << "Success open file" << std::endl;
    }
    else {
        std::cout << "File is not open" << std::endl;
    }

    json::Document doc = Load(ifile);

    TransportCatalogue catalogue;
    Reader reader(doc.GetRoot());
    reader.LoadData();

    std::ofstream ofile("otest.txt");
    reader.ProcessQuery(ofile);


}

int main() {
    // Reader_test();

    json::Document doc = Load(std::cin);
    TransportCatalogue catalogue;
    Reader reader(doc.GetRoot());
    reader.LoadData();
    reader.ProcessQuery(std::cout);


}