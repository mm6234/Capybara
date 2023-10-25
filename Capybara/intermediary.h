// Intermediary pulls from the database

#pragma once
#include <iostream>				// Does not and should not have drogon
#include <json/json.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

using namespace std;

tuple <int, string> doctorInfo(const string id);

// Accepts an ID and returns Doctor JSON Data
Json::Value getDataById(const string id);

tuple <int, string> update(const nlohmann::json parsedJson);

// Update Doctor Database
void updateDoctorDatabase(int doctorId, const string& fieldToUpdate, const string& fieldValue);

// Create a new resource to the database
int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue);