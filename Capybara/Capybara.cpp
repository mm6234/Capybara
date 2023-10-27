#include "Capybara.h"
#include "intermediary.h"
#include <drogon/drogon.h>          // does not json.h i assume
#include <fstream>
#include <filesystem>          // PWD
#include <nlohmann/json.hpp>
#include <sqlite3.h>
using namespace std;
using namespace drogon;

string capybanner = R"(  ,--,    .--.  ,---..-.   .-.,---.     .--.  ,---.    .--.1.0
.' .')   / /\ \ | .-.\\ \_/ )/| .-.\   / /\ \ | .-.\  / /\ \  
|  |(_) / /__\ \| |-' )\   (_)| |-' \ / /__\ \| `-'/ / /__\ \ 
\  \    |  __  || |--'  ) (   | |--. \|  __  ||   (  |  __  | 
 \  `-. | |  |)|| |     | |   | |`-' /| |  |)|| |\ \ | |  |)| 
  \____\|_|  (_)/(     /(_|   /( `--' |_|  (_)|_| \)\|_|  (_) 
               (__)   (__)   (__)                 (__)        )";

tuple <string, string> queryGetFieldFromValue(string ratingVal, string ratingSubmissionsVal, string locationVal) {
    if (ratingVal == "Yes") {
        return make_tuple("rating", ratingVal);
    }
    if (ratingSubmissionsVal == "Yes") {
        return make_tuple("ratingSubmissions", ratingSubmissionsVal);
    }
    if (locationVal != "") {
        return make_tuple("location", locationVal);
    }
    return make_tuple("", "");
}


HttpStatusCode convertStatusCode(int number) {
    if (number == 200) {
        return HttpStatusCode::k200OK;
    }
    else {
        return HttpStatusCode::k400BadRequest;
    }
}

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
                char* error;
                sqlite3* db;
                sqlite3_stmt* stmt;
                sqlite3_open("db.db", &db);
                int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS doctorInfo(\
                            id INT, \
                            doctorName varchar(100), \
                            rating decimal(18,4), \
                            ratingSubmissions INT, \
                            latitude decimal(18,4), \
                            longitude decimal(18,4), \
                            practiceKeywords varchar(255), \
                            languagesSpoken varchar(255), \
                            insurance varchar(255), \
                            streetAddress varchar(255) \
                            );", NULL, NULL, &error);
                if (rc != SQLITE_OK) {
                    cout << "error creating database" << endl;
                }

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
              auto [statusCode, stringBody] = doctorInfo(id);
              auto resp = drogon::HttpResponse::newHttpResponse();
              resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
              
              resp->setStatusCode(convertStatusCode(statusCode));
              resp->setBody(stringBody);
              callback(resp);
        },
        { Get }
    );

    //POST https://capybara.com/api/doctor-info/api/update
    //Ingests JSON Doctor Information -> Updates Doctor DB -> returns Success Page
    app().registerHandler("/api/update",
        [](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                try {
                    nlohmann::json parsedJson = nlohmann::json::parse(req->body());
                    bool parsed = true;
                    if (parsed == true) {
                        auto [statusCode, stringBody] = update(parsedJson);
                        resp->setStatusCode(convertStatusCode(statusCode));
                        resp->setBody(stringBody);
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

                callback(resp);
        },
        { drogon::Post }
    );



    app().registerHandler("/api/query/{ratingVal}/{ratingSubmissionsVal}/{location}",
        [](const HttpRequestPtr& req,
            function<void(const HttpResponsePtr&)>&& callback,
            const string& ratingVal,
            const string& ratingSubmissionsVal, 
            const string& locationVal) {
                auto resp = HttpResponse::newHttpResponse();
                auto [field, value] = queryGetFieldFromValue(ratingVal, ratingSubmissionsVal, locationVal);
                if (field == "" && value == "") {
                    resp->setStatusCode(HttpStatusCode::k400BadRequest);
                    resp->setBody("{\"error\": \"Invalid request\"}");
                }
                auto [statusCode, stringBody] = query(field, value);

                resp->setStatusCode(HttpStatusCode::k200OK);
                resp->setBody(stringBody);
                callback(resp);
        },
        { Get });
 
        

    app().run();
}