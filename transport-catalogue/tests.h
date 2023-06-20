#pragma once

#include <iomanip>
#include <fstream>
#include <iostream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

void Test_filling(TransportCatalogue& catalog) {
	std::ifstream ifile("test.txt");
	if (ifile.is_open()) {
		std::cout << "Success open file" << std::endl;
	}
	else {
		std::cout << "File is not open" << std::endl;
	}
	LoadTC(catalog, ifile);
	ifile.close();
	std::cout << "Filling test is successfully completed" << std::endl;
}

void Test_stat(TransportCatalogue& catalog) {
	std::ifstream ifile( "test2.txt" );
	if (ifile.is_open()) {
		std::cout << "Success open file" << std::endl;
	}
	else {
		std::cout << "File is not open" << std::endl;
	}
	Query_processing(catalog, ifile);
	std::cout << "Stating test is successfully completed" << std::endl;
}

void Test(){
	//std::cout<<std::setprecision(6);
	TransportCatalogue catalog;
	Test_filling(catalog);
	Test_stat(catalog);
}