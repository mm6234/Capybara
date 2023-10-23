// Intermediary pulls from the database

#pragma once
#include <iostream>
#include <drogon/drogon.h>          // does not json.h i assume

using namespace std;
using namespace drogon;

HttpResponsePtr doctorInfo(const drogon::HttpRequestPtr& req, const string id);

// Accepts an ID and returns Doctor JSON Data
Json::Value getDataById(const string id);

HttpResponsePtr update(const drogon::HttpRequestPtr& req);

// Update Doctor Database
void updateDoctorDatabase(int doctorId, const string& fieldToUpdate, const string& fieldValue);

// Create a new resource to the database
int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue);