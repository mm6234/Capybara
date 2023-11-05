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

	//Get /api/doctor-info/{DoctorID}
	tuple <int, string> doctorInfo(const string id);

};


// -------------------------------
void db_init();

//tuple <int, string> doctorInfo(const string id);

// Accepts an ID and returns Doctor JSON Data
Json::Value getDataById(const string id);

tuple <int, string> update(const nlohmann::json parsedJson);

// Update Doctor Database
int updateDoctorDatabase(string doctorId, string& fieldToUpdate, string& fieldValue);

// Create a new resource to the database
int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue);

tuple<int, string>  query(string field, string value);


// Admin Tools -- for Testing
// Deletes db (for Testing)
void _db_destroy();