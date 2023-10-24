// Intermediary pulls from the database

#pragma once
#include <iostream>				// Does not and should not have drogon
#include <json/json.h>
#include <unordered_map>

using namespace std;

// Accepts an ID and returns Doctor JSON Data
Json::Value getDataById(string id);

// Update Doctor Database
std::unordered_map<int, Json::Value> updateDoctorDatabase(int doctorId, const std::string& fieldToUpdate, const std::string& fieldValue);