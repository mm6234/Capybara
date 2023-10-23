#include "intermediary.h"
#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace std;

HttpResponsePtr doctorInfo(const drogon::HttpRequestPtr& req, const string id) {
   auto data = getDataById(id);
   if (data == NULL) {
     auto resp = drogon::HttpResponse::newHttpResponse();
     resp->setStatusCode(HttpStatusCode::k400BadRequest);
     resp->setBody("{\"error\": \"Illegal 'id' field!\"}");
     return resp;
     }
     auto resp = drogon::HttpResponse::newHttpJsonResponse(data);
     resp->setStatusCode(HttpStatusCode::k200OK);
     return resp;
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


HttpResponsePtr update(const drogon::HttpRequestPtr& req) {
    HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);

    try {
        auto parsedJson = nlohmann::json::parse(req->body());
        if (parsedJson.find("fieldValue") != parsedJson.end()) {
        }
        if (parsedJson.find("id") != parsedJson.end() &&
            parsedJson.find("fieldToUpdate") != parsedJson.end() &&
            parsedJson.find("fieldValue") != parsedJson.end()) {
            // id field exists, so we update the existing record
            int doctorId = parsedJson["id"].get<int>();
            string fieldValue = parsedJson["fieldValue"].get<string>();
            string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();

            updateDoctorDatabase(doctorId, fieldToUpdate, fieldValue);
            resp->setStatusCode(HttpStatusCode::k200OK);
        }
        else if (parsedJson.find("fieldToUpdate") != parsedJson.end() &&
            parsedJson.find("fieldValue") != parsedJson.end()) {
            // id field does not exist, so we create a new record
            string fieldValue = parsedJson["fieldValue"].get<string>();
            string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();
            string id = to_string(updateCreateNewRecord(fieldToUpdate, fieldValue));
            resp->setStatusCode(HttpStatusCode::k200OK);
            resp->setBody("{\"id\": " + id + "}");
        }
        else {
            resp->setStatusCode(HttpStatusCode::k400BadRequest);
            resp->setBody("{\"error\": \"Invalid JSON format in the request body\"}");
        }
    }
    catch (const std::exception& e) {
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setBody("{\"error\": \"Invalid JSON format in the request body\"}");
    }
    catch (...) {
        resp->setStatusCode(HttpStatusCode::k400BadRequest);
        resp->setBody("{\"error\": \"Unknown error occurred\"}");
    }
    return resp;
}

void updateDoctorDatabase(int doctorId, const std::string& fieldToUpdate, const std::string& fieldValue) {
  // todo: update the doctor database here
}

int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue) {
    // todo: create a new doctor record here with the provided field
    // ex: we create a new record with doctorName "Capybara", then 
    return 1;
}