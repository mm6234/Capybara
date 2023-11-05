#include "Capybara.h"
#include "intermediary.h"

int main()
{
    // DB Initializer
    Intermediary db;
    cout << "[+] DB Initialized!" << endl;

    app().addListener("0.0.0.0", 6969);
    cout << "[+] Created Listener" << endl;

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
        [&](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback,
            const std::string& id) {
              //auto [statusCode, stringBody] = doctorInfo(id);
                auto [statusCode, stringBody] = db.doctorInfo(id);

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
 

    cout << "[+] Registered Handlers" << endl;
    cout << "[+] Service is now Running!" << endl;
    app().run();
    cout << "[+] Service is Done Running!" << endl;
}