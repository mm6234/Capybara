#include "intermediary.h"

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

Intermediary::Intermediary(DatabaseAbstract* iv) : iv_(iv) {
    // initalizes Database fields
    char* error;
    sqlite3_open("db.db", &this->db);
    int rc = sqlite3_exec(this->db, "CREATE TABLE IF NOT EXISTS doctorInfo(\
                            id INT, \
                            doctorName varchar(100), \
                            rating decimal(18,4), \
                            ratingSubmissions INT, \
                            latitude decimal(18,4), \
                            longitude decimal(18,4), \
                            practiceKeywords varchar(255), \
                            languagesSpoken varchar(255), \
                            insurance varchar(255), \
                            streetAddress varchar(255), \
                            clientUserName varchar(100) \
                            );", NULL, NULL, &error);
    
    int rc2 = sqlite3_exec(this->db, "CREATE TABLE IF NOT EXISTS clientInfo(\
                            clientUserName varchar(100) \
                            );", NULL, NULL, &error);

    if (rc != SQLITE_OK || rc2 != SQLITE_OK) {
        cout << "[-] Error Initializing Database!" << endl;
        cerr << error << endl;
    }
}

Intermediary::~Intermediary() {
    sqlite3_close(this->db);
    cout << "[+] Successfully Closed Database!" << endl;
}

tuple <int, string> Intermediary::doctorInfo(const string id) {
    auto data = iv_->getDataById(id);
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

tuple <int, string> Intermediary::update(const nlohmann::json parsedJson) {
    if (parsedJson.find("id") != parsedJson.end() &&
        parsedJson.find("fieldToUpdate") != parsedJson.end() &&
        parsedJson.find("fieldValue") != parsedJson.end() &&
        parsedJson.find("clientUserName") != parsedJson.end()) {
        // id field exists, so we update the existing record

        int doctorId = parsedJson["id"].get<int>();
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();
        string clientUserName = parsedJson["clientUserName"].get<string>();

        int result = iv_->updateDoctorDatabase(to_string(doctorId), fieldToUpdate, fieldValue, clientUserName);
        if (result == 0) { return make_tuple(200, ""); }
        else { return make_tuple(400, "{\"error\": \"Client does not have permission, or other database error\"}"); }

    }
    else if (parsedJson.find("fieldToUpdate") != parsedJson.end() &&
        parsedJson.find("fieldValue") != parsedJson.end() &&
        parsedJson.find("clientUserName") != parsedJson.end()) {
        // id field does not exist, so we create a new record
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();
        string clientUserName = parsedJson["clientUserName"].get<string>();
        string id = to_string(iv_->updateCreateNewRecord(fieldToUpdate, fieldValue, clientUserName));
        if (id == "-1") { return make_tuple(400, "{\"error\": \"Client username is not registered, or other database error\"}"); }
        return make_tuple(200, "{\"id\": " + id + "}");
    }
    else { return make_tuple(400, "{\"error\": \"Invalid JSON format in the request body\"}"); }

}

tuple<int, string> Intermediary::query(string field, string value) {
    string query;
    if (field == "rating" || field == "ratingSubmissions") {
        query = "select * from doctorInfo order by " + field + " desc;";
    }
    else if (field == "location") {

        vector<string> v = iv_->split(value, "_");
        if (v.size() != 2) { return make_tuple(400, "{\"error\": \"Wrong location format.\"}"); }
        try {
            float a = stof(v[0]);
            float b = stof(v[1]);
        }
        catch (...) { return make_tuple(400, "{\"error\": \"Wrong location format.\"}"); }
        query = "select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-" + v[0] + ")*(latitude-" + v[0] + ")+(longitude-" + v[1] + ")*(longitude-" + v[1] + ") as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;";

    }
    auto data = iv_->getDataByQuery(query);
    if (data == NULL) {
        return make_tuple(400, "{\"error\": \"No available doctors or wrong request format\"}");
    }
    else {
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        string dataString = Json::writeString(builder, data);
        return make_tuple(200, dataString);
    }
}

tuple<int, string> Intermediary::registerClient(const nlohmann::json parsedJson) {
    if (parsedJson.find("clientUserName") != parsedJson.end()) {
        int result = iv_->registerClientNewRecord(parsedJson["clientUserName"].get<string>());

        if (result == 0) { return make_tuple(200, ""); }
        else { return make_tuple(400, "{\"error\": \"Unknown error occurred\"}"); }
    }
    else { return make_tuple(400, "{\"error\": \"Invalid JSON format in the request body\"}"); }
}
