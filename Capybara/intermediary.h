// Intermediary pulls from the database
// Does not and should not have drogon
#pragma once		
#include <json/json.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <stdio.h>
#include <tuple>
#include <iostream>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>				// for testing

using namespace std;

// Transitioning to OOP
class Intermediary {
public:
	// Constructor, initializes the database
	Intermediary();

	//~Intermediary();		// DESTRUCTOR - TODO

	//Get /api/doctor-info/{DoctorID}
	tuple <int, string> doctorInfo(const string id);

	tuple <int, string> update(const nlohmann::json parsedJson);

	tuple<int, string> query(string field, string value);

private:
	// Accepts an ID and returns Doctor JSON Data
	Json::Value getDataById(const string id);

	static int select_callback(void* ptr, int argc, char* argv[], char* cols[]);

	// Update Doctor Database
	int updateDoctorDatabase(string doctorId, std::string& fieldToUpdate, std::string& fieldValue);
	
	// Create a new resource to the database
	int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue);

	vector<string> split(string str, string token);
};