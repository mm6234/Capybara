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
#include <string.h>
#include "databaseAbstract.h"

using namespace std;

// Transitioning to OOP
class Intermediary {
	DatabaseAbstract* iv_;
public:
	Intermediary(DatabaseAbstract* iv);		// initializes the database, TODO: handle case if fail to open

	~Intermediary();

	// Accepts an ID and returns Doctor JSON Data
	tuple <int, string> doctorInfo(const string id);

	tuple <int, string> update(const nlohmann::json parsedJson);

	tuple<int, string> query(string field, string value);

	// delete doctor functionality

protected:	// MAKE SURE TO USE THIS-> TO REFERENCE PROTECTED VARS/FUNCTIONS
	sqlite3* db;

	static int select_callback(void* ptr, int argc, char* argv[], char* cols[]);

};