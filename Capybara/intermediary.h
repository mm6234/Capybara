// Intermediary pulls from the database

#pragma once
#include <iostream>
#include <drogon/drogon.h>          // does not json.h i assume
#include <fstream>
#include <filesystem>          // PWD
#include <nlohmann/json.hpp>

using namespace std;

// Get ID
Json::Value getDataById(string id);

// Update Doctor Database
std::unordered_map<int, Json::Value> updateDoctorDatabase(int doctorId, const std::string& fieldToUpdate, const std::string& fieldValue);