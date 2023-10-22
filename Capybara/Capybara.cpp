﻿
#include "Capybara.h"
#include "intermediary.h"
#include <drogon/drogon.h>          // does not json.h i assume
#include <fstream>
#include <filesystem>          // PWD
#include <nlohmann/json.hpp>
using namespace std;
using namespace drogon;

string capybanner = R"(  ,--,    .--.  ,---..-.   .-.,---.     .--.  ,---.    .--.1.0
.' .')   / /\ \ | .-.\\ \_/ )/| .-.\   / /\ \ | .-.\  / /\ \  
|  |(_) / /__\ \| |-' )\   (_)| |-' \ / /__\ \| `-'/ / /__\ \ 
\  \    |  __  || |--'  ) (   | |--. \|  __  ||   (  |  __  | 
 \  `-. | |  |)|| |     | |   | |`-' /| |  |)|| |\ \ | |  |)| 
  \____\|_|  (_)/(     /(_|   /( `--' |_|  (_)|_| \)\|_|  (_) 
               (__)   (__)   (__)                 (__)        )";

int main()
{
    app().addListener("0.0.0.0", 6969);

    // GET Index Page
    app().registerHandler(
        "/",
        [](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_TEXT_HTML);

                string responseBody = "<html><head><title>Capybara</title></head><body><h1>Welcome to Capybara!</h1></body></html>\n" + capybanner;
                resp->setBody(responseBody);
                callback(resp);
        }
    );

    //GET https://capybara.com/api/doctor-info/{DoctorId}
    app().registerHandler("/api/doctor-info/{id}",
        [](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback,
            const std::string& id) {
                try {
                    auto data = getDataById(id);
                    if (data == NULL) {
                        auto resp = drogon::HttpResponse::newHttpResponse();
                        resp->setStatusCode(HttpStatusCode::k400BadRequest);
                        resp->setBody("{\"error\": \"Illegal 'id' field!\"}");
                        callback(resp);
                    }
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(data);
                    resp->setStatusCode(HttpStatusCode::k200OK);
                    callback(resp);
                }
                catch (const std::exception& e) {
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(HttpStatusCode::k400BadRequest);
                    resp->setBody("{\"error\": \"'id' field is missing.\"}");
                    callback(resp);

                }

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
                        //Capybara c;
                        Json::Value doctorData = getDataById("3");

                        //c.updateDoctorDatabase(doctorId, fieldToUpdate, fieldValue);
                        updateDoctorDatabase(doctorId, fieldToUpdate, fieldValue);
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


    //// GET https://capybara.com/api/query?field1=value1&field2=value2           // Will need to experiment to see if it can take n fields
    //app().registerHandler("/api/query?field1={val1}&field2={val2}",
    //    [](const HttpRequestPtr& req,
    //        function<void(const HttpResponsePtr&)>&& callback,
    //        const string& val1,
    //        const string& val2) { 

    //            Json::Value json;
    //            json["field1"] = val1;
    //            json["field2"] = val2;
    //            cout << "JSON: " << json << endl;                                               // RCVD FORMATTED JSON, can do whatever with it

    //            auto resp = HttpResponse::newHttpResponse();
    //            //auto resp = HttpResponse::newHttpJsonResponse(json);
    //            callback(resp);
    //    },
    //    { Get });
 

    //app().registerHandler("/api/doctor-info/{id}",
    //    [](const drogon::HttpRequestPtr& req,
    //        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    //        const std::string& id) {
    //            cout << "RCVD ID: " << id << endl;
    //            Capybara c;
    //            LinearRegressionModel l;
    //            auto resp = drogon::HttpResponse::newHttpJsonResponse(c.train(int 100, float 0.1)(id));
    //            callback(resp);
    //    },
    //    { Get }
    //);


    app().run();
}