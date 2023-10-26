#include "intermediary.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

using namespace std;

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

int select_callback(void* ptr, int argc, char* argv[], char* cols[])
// https://stackoverflow.com/questions/15836253/c-populate-vector-from-sqlite3-callback-function
{
    Records* table = static_cast<Records*>(ptr);
    vector<string> row;
    for (int i = 0; i < argc; i++)
        row.push_back(argv[i] ? argv[i] : "NULL");
    table->push_back(row);
    return 0;
}

tuple <int, string> doctorInfo(const string id) {
   auto data = getDataById(id);
   if (data == NULL) {
     return make_tuple(400, "{\"error\": \"Illegal 'id' field!\"}");
     }
   else {
       Json::StreamWriterBuilder builder;
       builder["indentation"] = "";
       string dataString = Json::writeString(builder, data);
       return make_tuple(200, dataString);
   }
}

Json::Value getDataById(const string id) {
    //TODO: Sql query to get all values
    int id_int;
    try {
        id_int = stoi(id);
    }
    catch (const std::exception& e){
        return NULL;
    }


    Json::Value location;
    location["latitude"] = 3.222;
    location["longitude"] = 78.43;
    Json::Value other;
    other["streetAddress"] = "NYC";

    Json::Value data;
    data["id"] = 3;
    data["doctorName"] = "Capybara";
    data["rating"] = 3.5;
    data["ratingSubmissions"] = 99;
    data["location"] = location;
    data["practiceKeywords"] = Json::arrayValue;
    data["practiceKeywords"].append("Ear");
    data["practiceKeywords"].append("Nose");
    data["practiceKeywords"].append("Throat");
    data["languagesSpoken"] = Json::arrayValue;
    data["languagesSpoken"].append("English");
    data["insurance"] = Json::arrayValue;
    data["languagesSpoken"].append("Aetna");
    data["other"] = other;

    return data;
}


tuple <int, string> update(const nlohmann::json parsedJson) {
    
    if (parsedJson.find("id") != parsedJson.end() &&
    parsedJson.find("fieldToUpdate") != parsedJson.end() &&
    parsedJson.find("fieldValue") != parsedJson.end()) {
        // id field exists, so we update the existing record
        
        int doctorId = parsedJson["id"].get<int>();
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();

        updateDoctorDatabase(to_string(doctorId), fieldToUpdate, fieldValue);
        return make_tuple(200, "");
    }
    else if (parsedJson.find("fieldToUpdate") != parsedJson.end() &&
    parsedJson.find("fieldValue") != parsedJson.end()) {
        // id field does not exist, so we create a new record
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();
        string id = to_string(updateCreateNewRecord(fieldToUpdate, fieldValue));
        return make_tuple(200, "{\"id\": " + id + "}");
        }
    else {
        return make_tuple(400, "{\"error\": \"Invalid JSON format in the request body\"}");
        }
    
}

void updateDoctorDatabase(string doctorId, std::string& fieldToUpdate, std::string& fieldValue) {
    char* error;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int opened = sqlite3_open("db.db", &db);
    Records records;
    if (fieldToUpdate == "doctorName" || fieldToUpdate == "practiceKeywords" || \
        fieldToUpdate == "languagesSpoken" || fieldToUpdate == "insurance" || \
        fieldToUpdate == "streetAddress") {
        fieldValue = "\'" + fieldValue + "\'";
    }

    string query = "update doctorInfo set " + fieldToUpdate + " = " + fieldValue + " where id = " + doctorId + ";";
    int exec1 = sqlite3_exec(db, query.c_str(), NULL, NULL, &error);
}

int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue) {
    char* error;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int opened = sqlite3_open("db.db", &db);
    Records records;
    int exec1 = sqlite3_exec(db, "select count(*) from doctorInfo", select_callback, &records, &error);
    int newId = stoi(records[0][0]) + 1;

    vector<string> values(10, "NULL, ");
    values[0] = to_string(newId)+", ";
    if (fieldToUpdate == "doctorName") {
        values[1] = "\'" + fieldValue + "\', ";
    }
    string query = "insert into doctorInfo VALUES (";
    for (const auto& e : values) query += e;
    query.pop_back();
    query.pop_back();
    query += ");";
    int exec2 = sqlite3_exec(db, query.c_str(), NULL, NULL, &error);

    return newId;
}