// Intermediary pulls from the database

#pragma once
#include <iostream>
#include <drogon/drogon.h>          // does not json.h i assume

using namespace std;

// Accepts an ID and returns Doctor JSON Data
Json::Value getDataById(string id);

// Update Doctor Database
std::unordered_map<int, Json::Value> updateDoctorDatabase(int doctorId, const std::string& fieldToUpdate, const std::string& fieldValue);