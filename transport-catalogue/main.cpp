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
#include "json_builder.h"

using namespace json;
using namespace std::literals;


using namespace std;

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
    JSONReader reader(doc.GetRoot());
    reader.LoadData();

    std::ofstream ofile("otest.txt");
    reader.ProcessQuery(std::cout);


}


void PrintTest() {
    json::Print(
        json::Document{
        json::Builder{}
        .StartDict()
            .Key("key1"s).Value(123)
            .Key("key2"s).Value("value2"s)
            .Key("key3"s).StartArray()
            .Value(456)
            .StartDict().EndDict()
            .StartDict()
            .Key(""s)
            .Value(nullptr)
            .EndDict()
            .Value(""s)
            .EndArray()
            .EndDict()
            .Build()
    },
        cout
    );
    cout << endl;

    json::Print(
        json::Document{
        json::Builder{}
        .Value("just a string"s)
            .Build()
    },
        cout
    );
    cout << endl;

    cout << "----------------------Print test done----------------------" << endl;
}


void ErrorTest() {
   //json::Builder{}.StartDict().Build();  // правило 3
   //json::Builder{}.StartDict().Key("1"s).Value(1).Value(1);  // правило 2
   //json::Builder{}.StartDict().Key("1"s).Key(""s);  // правило 1
   //json::Builder{}.StartArray().Key("1"s);  // правило 4
   //json::Builder{}.StartArray().EndDict();  // правило 4
   //json::Builder{}.StartArray().Value(1).Value(2).EndDict();  // правило 5
}

void Test() {
    PrintTest();
    Reader_test();
    ErrorTest();
}
int main() {
   // Test();

    json::Document doc = Load(cin);

    TransportCatalogue catalogue;
    JSONReader reader(doc.GetRoot());
    reader.LoadData();

    reader.ProcessQuery(cout);


}