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
    // No Input -> Returns Welcome Page
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
    //DoctorID MUST be an integer -> returns information on the doctor
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

    //POST https://capybara.com/api/doctor-info/api/update
    //Ingests JSON Doctor Information -> Updates Doctor DB -> returns Success Page
    app().registerHandler(
        "/api/update",
        [](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                HttpResponsePtr resp = update(req);
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