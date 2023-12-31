#include "Capybara.h"
#include "intermediary.h"
#include"databaseAbstract.h"
#include"database.h"

int main()
{   
    app().registerPreRoutingAdvice([](const auto& req, auto&& callback, auto&& chainCallback) {
        auto response = drogon::HttpResponse::newHttpResponse();
        // Allow requests from any origin
        response->addHeader("Access-Control-Allow-Origin", "*");
        // Allow specific headers in the request
        response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
        // Allow specific HTTP methods
        response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");

        if (req->getMethod() == drogon::HttpMethod::Options) {
            response->setStatusCode(k200OK);
            callback(response);
        }
        else {
            chainCallback();
        }
    });    
    // DB Initializer
    DatabaseAbstract* database = new Database();
    Intermediary db(database);
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
        [&](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                try {
                    nlohmann::json parsedJson = nlohmann::json::parse(req->body());
                    bool parsed = true;
                    if (parsed == true) {
                        auto [statusCode, stringBody] = db.update(parsedJson);
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
        [&](const HttpRequestPtr& req,
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
                auto [statusCode, stringBody] = db.query(field, value);

                resp->setStatusCode(HttpStatusCode::k200OK);
                resp->setBody(stringBody);
                callback(resp);
        },
        { Get });

    //POST https://capybara.com/api/doctor-info/api/register
    app().registerHandler("/api/register",
        [&](const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                try {
                    nlohmann::json parsedJson = nlohmann::json::parse(req->body());
                    auto [statusCode, stringBody] = db.registerClient(parsedJson);
                    resp->setStatusCode(convertStatusCode(statusCode));
                    resp->setBody(stringBody);
                }
                catch (...) {
                    resp->setStatusCode(HttpStatusCode::k400BadRequest);
                    resp->setBody("{\"error\": \"Invalid JSON format in the request body\"}");
                }

                callback(resp);
        },
        { drogon::Post }
    );
    
    cout << "[+] Registered Handlers" << endl;
    cout << "[+] Service is now Running!" << endl;
    app().run();
    cout << "[+] Service is Done Running!" << endl;
}