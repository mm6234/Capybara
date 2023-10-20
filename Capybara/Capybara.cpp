
#include "Capybara.h"
#include <drogon/drogon.h>          // does not json.h i assume
#include <fstream>
#include <filesystem>          // PWD
#include <nlohmann/json.hpp>
using namespace std;
using namespace drogon;

    std::unordered_map<int, Json::Value> doctorDatabase;

    void updateDoctorDatabase(int doctorId, const std::string& fieldToUpdate, const std::string& fieldValue) {
        if (doctorDatabase.find(doctorId) != doctorDatabase.end()) {
            doctorDatabase[doctorId][fieldToUpdate] = Json::Value(fieldValue);
        }
        else {
            Json::Value newDoctor;
            newDoctor[fieldToUpdate] = Json::Value(fieldValue);
            doctorDatabase[doctorId] = newDoctor;
        }
    }

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

                try {
                    auto parsedJson = nlohmann::json::parse(req->body());

                    if (parsedJson.find("id") != parsedJson.end() &&
                        parsedJson.find("fieldToUpdate") != parsedJson.end() &&
                        !parsedJson["fieldToUpdate"].is_null()) {

                        int doctorId = parsedJson["id"].get<int>();
                        std::string fieldValue = parsedJson["fieldValue"].get<std::string>();
                        std::string fieldToUpdate = parsedJson["fieldToUpdate"].get<std::string>();

                        // Hardcoding the data into the doctorDatabase map for testing purposes
                        Capybara c;
                        Json::Value doctorData = c.getDataById("3");
                        doctorDatabase[3] = doctorData;

                        updateDoctorDatabase(doctorID, fieldToUpdate, fieldValue);
                        resp->setStatusCode(HttpStatusCode::k200OK);
                        resp->setBody("{\"status\": 200}");
                    }
                    else if(parsedJson.find("id") != parsedJson.end()){
                        resp->setStatusCode(HttpStatusCode::k400BadRequest);
                        resp->setBody("{\"error\": \"'id' field is missing.\"}");
                    }
                    else {
                        resp->setStatusCode(HttpStatusCode::k400BadRequest);
                        resp->setBody("{\"error\": \"fieldToUpdate not found.\"}");
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