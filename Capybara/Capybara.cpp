
#include "Capybara.h"
#include <drogon/drogon.h>          // includes json.h i assume
#include <fstream>
#include <filesystem>               // PWD

using namespace std;
using namespace drogon;

Json::Value getDataById(string id) {
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

                auto resp = drogon::HttpResponse::newHttpJsonResponse(getDataById(id));
                callback(resp);
        },
        { Get }
    );

    // POST https://capybara.com/api/update
    app().registerHandler(
        "/api/update",
        [](const drogon::HttpRequestPtr& req,
            function<void(const drogon::HttpResponsePtr&)>&& callback) {
                auto resp = drogon::HttpResponse::newHttpResponse();            // Creates Response Object
                resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);                // set ContentType to text/plain

                // Get the POST data from the request body
                auto postData = req->body();                                    // What was rcvd from the body

                // Set the response content as the POST data
                string temp = string(postData);
                resp->setBody(temp);                                            // Sends the RCVD POST data back to the user
                //resp->setBody("Get posted!");


                cout << "RCVD: " << postData << endl;                           // POST data is here, can do whatever now
                callback(resp);                                                 // Sends the constructed obj back to the client as a response
        },
        { Post }
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
