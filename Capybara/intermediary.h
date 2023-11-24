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
	Intermediary();		// initializes the database, TODO: handle case if fail to open

	~Intermediary();

	// Accepts an ID and returns Doctor JSON Data
	virtual tuple <int, string> doctorInfo(const string id);

	virtual tuple <int, string> update(const nlohmann::json parsedJson);

	virtual tuple<int, string> query(string field, string value);

	// delete doctor functionality

private:	// MAKE SURE TO USE THIS-> TO REFERENCE PRIVATE VARS/FUNCTIONS
	sqlite3* db;

	// Helper Function for doctorInfo()
	Json::Value getDataById(const string id);

	static int select_callback(void* ptr, int argc, char* argv[], char* cols[]);

	// Update Doctor Database
	int updateDoctorDatabase(string doctorId, std::string& fieldToUpdate, std::string& fieldValue);
	
	// Create a new resource to the database
	int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue);

	vector<string> split(string str, string token);

};