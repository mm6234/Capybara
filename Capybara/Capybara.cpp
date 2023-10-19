
#include "Capybara.h"
#include <drogon/drogon.h>          // does not json.h i assume
#include <fstream>
#include <filesystem>          // PWD
#include <nlohmann/json.hpp>
using namespace std;
using namespace drogon;


    Json::Value Capybara::getDataById(string id) {
        //TODO: Sql query to get all values
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

int main()
{
    app().addListener("0.0.0.0", 6969);

    //GET https://capybara.com/api/doctor-info/{DoctorId}
    app().registerHandler("/api/doctor-info/{id}",
        [](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback,
            const std::string& id) {
                cout << "RCVD ID: " << id << endl;
                Capybara c;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(c.getDataById(id));
                callback(resp);
        },
        { Get }
    );

    app().registerHandler(
        "/api/update",
        [](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);

                auto postData = req->body();

                Json::Value requestBody;
                try {
                    auto parsedJson = nlohmann::json::parse(postData);;

                }
                catch (const std::exception& e) {
                    resp->setStatusCode(HttpStatusCode::k400BadRequest);
                    resp->setBody("{\"error\": \"Invalid JSON format in the request body\"}");
                    callback(resp);
                    return;
                }

                int doctorId;
                std::string fieldToUpdate;

                if (requestBody.isMember("id") && requestBody.isMember("fieldToUpdate") && !requestBody["fieldToUpdate"].isNull()) {
                    doctorId = requestBody["id"].asInt();
                    fieldToUpdate = requestBody["fieldToUpdate"].asString();
                }
                else {
                    resp->setStatusCode(HttpStatusCode::k400BadRequest);
                    std::string errorMessage = "fieldToUpdate not found. ";
                    if (!requestBody.isMember("id")) {
                        errorMessage += "'id' field is missing. ";
                    }
                    resp->setBody("{\"error\": \"" + errorMessage + "\"}");
                    callback(resp);
                    return;
                }

                // Hardcoding the data into the doctorDatabase map for testing purposes
                Capybara c;
                static std::unordered_map<int, Json::Value> doctorDatabase = {
                    {3, c.getDataById("3")}
                };
                // map to fake a database till we develop it just for testing

                if (doctorDatabase.find(doctorId) != doctorDatabase.end()) {
                    if (doctorDatabase[doctorId].isMember(fieldToUpdate)) {
                        if (doctorDatabase[doctorId][fieldToUpdate].isArray()) {
                            doctorDatabase[doctorId][fieldToUpdate].append(requestBody[fieldToUpdate]);
                        }
                        else {
                            doctorDatabase[doctorId][fieldToUpdate] = requestBody[fieldToUpdate];
                        }
                    }
                    else {
                        doctorDatabase[doctorId][fieldToUpdate] = requestBody[fieldToUpdate];
                    }
                }
                else {
                    doctorDatabase[doctorId][fieldToUpdate] = requestBody[fieldToUpdate];
                }

                resp->setStatusCode(HttpStatusCode::k200OK);
                resp->setBody("{\"status\": 200}");
                callback(resp);
        },
        { drogon::Post }
    );







    // GET https://capybara.com/api/query?field1=value1&field2=value2           // Will need to experiment to see if it can take n fields
    app().registerHandler("/api/query?field1={val1}&field2={val2}",
        [](const HttpRequestPtr& req,
            function<void(const HttpResponsePtr&)>&& callback,
            const string& val1,
            const string& val2) { 

                Json::Value json;
                json["field1"] = val1;
                json["field2"] = val2;
                cout << "JSON: " << json << endl;                                               // RCVD FORMATTED JSON, can do whatever with it

                auto resp = HttpResponse::newHttpResponse();
                //auto resp = HttpResponse::newHttpJsonResponse(json);
                callback(resp);
        },
        { Get });


    app().run();
}