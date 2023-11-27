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

using namespace std;

class DatabaseAbstract {
public:
	virtual ~DatabaseAbstract() {};

	virtual Json::Value getDataById(const string id) = 0;
	// does not include callback, since we would not be testing it
	virtual int updateDoctorDatabase(string doctorId, std::string& fieldToUpdate, std::string& fieldValue) = 0;
	virtual int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue) = 0;
	virtual Json::Value getDataByQuery(string query) = 0;
	virtual vector<string> split(string str, string token) = 0;

};
