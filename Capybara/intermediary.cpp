#include "intermediary.h"

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

// ------------------------------------ OOP Transition
Intermediary::Intermediary() {
    // initalizes Database fields
    char* error;
    sqlite3* db;
    //sqlite3_stmt* stmt;
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
}

tuple <int, string> Intermediary::doctorInfo(const string id) {
    auto data = getDataById(id);
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

// ------------------------------------------
//void db_init() {
//    // initalizes Database fields
//    char* error;
//    sqlite3* db;
//    //sqlite3_stmt* stmt;
//    sqlite3_open("db.db", &db);
//    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS doctorInfo(\
//                            id INT, \
//                            doctorName varchar(100), \
//                            rating decimal(18,4), \
//                            ratingSubmissions INT, \
//                            latitude decimal(18,4), \
//                            longitude decimal(18,4), \
//                            practiceKeywords varchar(255), \
//                            languagesSpoken varchar(255), \
//                            insurance varchar(255), \
//                            streetAddress varchar(255) \
//                            );", NULL, NULL, &error);
//    if (rc != SQLITE_OK) {
//        cout << "error creating database" << endl;
//    }
//}

int select_callback(void* ptr, int argc, char* argv[], char* cols[])
// https://stackoverflow.com/questions/15836253/c-populate-vector-from-sqlite3-callback-function
{
    Records* table = static_cast<Records*>(ptr);
    vector<string> row;
    for (int i = 0; i < argc; i++)
        row.push_back(argv[i] ? argv[i] : "NULL");
    table->push_back(row);
    return 0;
}

//tuple <int, string> doctorInfo(const string id) {
//    auto data = getDataById(id);
//    if (data == NULL) {
//        return make_tuple(400, "{\"error\": \"Illegal 'id' field!\"}");
//    }
//    else {
//        Json::StreamWriterBuilder builder;
//        builder["indentation"] = "";
//        string dataString = Json::writeString(builder, data);
//        return make_tuple(200, dataString);
//    }
//}

Json::Value getDataById(const string id) {
    int id_int;
    try {
        id_int = stoi(id);
    }
    catch (...) {
        return NULL;
    }
    char* error;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int opened = sqlite3_open("db.db", &db);
    if (opened != SQLITE_OK) {
        return NULL;
    }
    Records records;
    string query = "select * from doctorInfo where id=" + id + " and id is not null;";
    int exec1 = sqlite3_exec(db, query.c_str(), select_callback, &records, &error);
    if (exec1 != SQLITE_OK || records.size()!=1) {
        return NULL;
    }
    Json::Value location;
    location["latitude"] = records[0][4];
    location["longitude"] = records[0][5];
    Json::Value other;
    other["streetAddress"] = records[0][9];

    Json::Value data;
    if (records[0][0] == "NULL"){
        data["id"] = "NULL";
    }
    else {
        data["id"] = stoi(records[0][0]);
    }
   
    data["doctorName"] = records[0][1];

    if (records[0][2] == "NULL") {
        data["rating"] = "NULL";
    }
    else {
        data["rating"] = stoi(records[0][2]);
    }
    if (records[0][3] == "NULL") {
        data["ratingSubmissions"] = "NULL";
    }
    else {
        data["ratingSubmissions"] = stoi(records[0][3]);
    }
    data["location"] = location;
    data["practiceKeywords"] = records[0][6];
    data["languagesSpoken"] = records[0][7];
    data["insurance"] = records[0][8];
    data["other"] = other;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    string dataString = Json::writeString(builder, data);

    return data;
}


tuple <int, string> update(const nlohmann::json parsedJson) {

    if (parsedJson.find("id") != parsedJson.end() &&
    parsedJson.find("fieldToUpdate") != parsedJson.end() &&
    parsedJson.find("fieldValue") != parsedJson.end()) {
        // id field exists, so we update the existing record
        
        int doctorId = parsedJson["id"].get<int>();
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();

        int result = updateDoctorDatabase(to_string(doctorId), fieldToUpdate, fieldValue);
        if (result == 0) {
            return make_tuple(200, "");
        }
        else {
            return make_tuple(400, "{\"error\": \"Unknown error occurred\"}");
        }
        
    }
    else if (parsedJson.find("fieldToUpdate") != parsedJson.end() &&
    parsedJson.find("fieldValue") != parsedJson.end()) {
        // id field does not exist, so we create a new record
        string fieldValue = parsedJson["fieldValue"].get<string>();
        string fieldToUpdate = parsedJson["fieldToUpdate"].get<string>();
        string id = to_string(updateCreateNewRecord(fieldToUpdate, fieldValue));
        return make_tuple(200, "{\"id\": " + id + "}");
        }
    else {
        return make_tuple(400, "{\"error\": \"Invalid JSON format in the request body\"}");
        }
    
}

int updateDoctorDatabase(string doctorId, std::string& fieldToUpdate, std::string& fieldValue) {
    char* error;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int opened = sqlite3_open("db.db", &db);
    Records records;
    if (fieldToUpdate == "doctorName" || fieldToUpdate == "practiceKeywords" || \
        fieldToUpdate == "languagesSpoken" || fieldToUpdate == "insurance" || \
        fieldToUpdate == "streetAddress") {
        fieldValue = "\'" + fieldValue + "\'";
    }

    string query = "update doctorInfo set " + fieldToUpdate + " = " + fieldValue + " where id = " + doctorId + ";";
    int exec1 = sqlite3_exec(db, query.c_str(), NULL, NULL, &error);
    if (exec1 != SQLITE_OK) { return 1; }
    Records records2;
    int exec3 = sqlite3_exec(db, "select * from doctorInfo", select_callback, &records2, &error);
    if (exec3 != SQLITE_OK) {return 1; }
    return 0;
}

int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue) {
    char* error;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int opened = sqlite3_open("db.db", &db);
    Records records;
    int exec1 = sqlite3_exec(db, "select count(*) from doctorInfo", select_callback, &records, &error);
    int newId = stoi(records[0][0]) + 1;

    vector<string> values(10, "NULL, ");
    values[0] = to_string(newId)+", ";
    if (fieldToUpdate == "doctorName") {
        values[1] = "\'" + fieldValue + "\', ";
    }
    else if (fieldToUpdate == "rating") {
        values[2] = fieldValue;
    }
    else if (fieldToUpdate == "ratingSubmissions") {
        values[3] = fieldValue;
    }
    else if (fieldToUpdate == "latitude") {
        values[4] = fieldValue;
    }
    else if (fieldToUpdate == "longitude") {
        values[5] = fieldValue;
    }
    else if (fieldToUpdate == "practiceKeywords") {
        values[6] = "\'" + fieldValue + "\', ";
    }
    else if (fieldToUpdate == "languagesSpoken") {
        values[7] = "\'" + fieldValue + "\', ";
    }
    else if (fieldToUpdate == "insurance") {
        values[8] = "\'" + fieldValue + "\', ";
    }
    else if (fieldToUpdate == "streetAddress") {
        values[9] = "\'" + fieldValue + "\', ";
    }
    string query = "insert into doctorInfo VALUES (";
    for (const auto& e : values) query += e;
    query.pop_back();
    query.pop_back();
    query += ");";
    int exec2 = sqlite3_exec(db, query.c_str(), NULL, NULL, &error);

    return newId;
}

vector<string> split(string str, string token) {
    // https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
    vector<string>result;
    while (str.size()) {
        int index = str.find(token);
        if (index != string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)result.push_back(str);
        }
        else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

tuple<int, string> query(string field, string value) {
    char* error;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int opened = sqlite3_open("db.db", &db);
    if (opened != SQLITE_OK) { return make_tuple(400, "{\"error\": \"Unknown error occurred\"}"); }
    Records records;
    string query;
    if (field == "rating" || field == "ratingSubmissions") {
        query = "select * from doctorInfo order by " + field + " desc;";
    }
    else if (field == "location") {
        
        vector<string> v = split(value, "_");
        if (v.size() != 2) { return make_tuple(400, "{\"error\": \"Wrong location format.\"}"); }
        try {
            float a = stof(v[0]);
            float b = stof(v[1]);
        }   
        catch (...) {
            return make_tuple(400, "{\"error\": \"Wrong location format.\"}");
        }
        query = "select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-"+v[0]+")*(latitude-" + v[0] + ")+(longitude-" + v[1] + ")*(longitude-" + v[1] + ") as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;";
        
    }
        
    int exec1 = sqlite3_exec(db, query.c_str(), select_callback, &records, &error);
    if (exec1 != SQLITE_OK) { return make_tuple(400, "{\"error\": \"Unknown error occurred\"}"); }
    if (records.size() == 0) { return make_tuple(400, "{\"error\": \"No available doctors or wrong request format\"}"); }
    Json::Value location;
    location["latitude"] = records[0][4];
    location["longitude"] = records[0][5];
    Json::Value other;
    other["streetAddress"] = records[0][9];

    Json::Value data;
    if (records[0][0] == "NULL") {
        data["id"] = "NULL";
    }
    else {
        data["id"] = stoi(records[0][0]);
    }

    data["doctorName"] = records[0][1];

    if (records[0][2] == "NULL") {
        data["rating"] = "NULL";
    }
    else {
        data["rating"] = stoi(records[0][2]);
    }
    if (records[0][3] == "NULL") {
        data["ratingSubmissions"] = "NULL";
    }
    else {
        data["ratingSubmissions"] = stoi(records[0][3]);
    }
    data["location"] = location;
    data["practiceKeywords"] = records[0][6];
    data["languagesSpoken"] = records[0][7];
    data["insurance"] = records[0][8];
    data["other"] = other;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    string dataString = Json::writeString(builder, data);
    return make_tuple(200, dataString);
}

void _db_destroy() {
    if (remove("db.db") != 0) {
        cout << "Failed to Destroy DB" << endl;
    }
    else {
        cout << "DB Destroyed!" << endl;
    }
}