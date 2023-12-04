#include "database.h"

Json::Value Database::getDataById(const std::string id) {
    int id_int;
    try {
        id_int = stoi(id);
    }
    catch (...) {
        return NULL;
    }
    char* error;
    Records records;
    string query = "select * from doctorInfo where id=" + id + " and id is not null;";
    int exec1 = sqlite3_exec(this->db, query.c_str(), this->select_callback, &records, &error);
    if (exec1 != SQLITE_OK || records.size() != 1) {
        return NULL;
    }
    Json::Value location;
    if (records[0][4] == "NULL") {
        location["latitude"] = "NULL";
    }
    else {
        location["latitude"] = stof(records[0][4]);
    }
    
    if (records[0][5] == "NULL") {
        location["longitude"] = "NULL";
    }
    else {
        location["longitude"] = stof(records[0][5]);
    }
    
    Json::Value other;
    other["streetAddress"] = records[0][9];

    Json::Value data;

    // we can directly use stoi because it cannot be NULL
    data["id"] = stoi(records[0][0]);

    data["doctorName"] = records[0][1];

    if (records[0][2] == "NULL") {
        data["rating"] = "NULL";
    }
    else {
        data["rating"] = stof(records[0][2]);
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

int Database::updateDoctorDatabase(std::string doctorId, std::string& fieldToUpdate, std::string& fieldValue) {
    char* error;
    Records records;
    if (fieldToUpdate == "doctorName" || fieldToUpdate == "practiceKeywords" || \
        fieldToUpdate == "languagesSpoken" || fieldToUpdate == "insurance" || \
        fieldToUpdate == "streetAddress") {
        fieldValue = "\'" + fieldValue + "\'";
    }

    string query = "update doctorInfo set " + fieldToUpdate + " = " + fieldValue + " where id = " + doctorId + ";";
    int exec1 = sqlite3_exec(this->db, query.c_str(), NULL, NULL, &error);
    if (exec1 != SQLITE_OK) { return 1; }
    Records records2;
    int exec3 = sqlite3_exec(this->db, "select * from doctorInfo", this->select_callback, &records2, &error);
    if (exec3 != SQLITE_OK) { return 1; }
    return 0;
}

int Database::updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue) {
    char* error;
    Records records;
    int exec1 = sqlite3_exec(this->db, "select count(*) from doctorInfo", select_callback, &records, &error);
    int newId = stoi(records[0][0]) + 1;

    vector<string> values(10, "NULL, ");
    values[0] = to_string(newId) + ", ";
    if (fieldToUpdate == "doctorName") {
        values[1] = "\'" + fieldValue + "\', ";
    }
    else if (fieldToUpdate == "rating") {
        values[2] = fieldValue + ", ";
    }
    else if (fieldToUpdate == "ratingSubmissions") {
        values[3] = fieldValue + ", ";
    }
    else if (fieldToUpdate == "latitude") {
        values[4] = fieldValue + ", ";
    }
    else if (fieldToUpdate == "longitude") {
        values[5] = fieldValue + ", ";
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
    int exec2 = sqlite3_exec(this->db, query.c_str(), NULL, NULL, &error);

    return newId;
}

Json::Value Database::getDataByQuery(string query) {
    char* error;
    Records records;

    int exec1 = sqlite3_exec(this->db, query.c_str(), this->select_callback, &records, &error);
    if (exec1 != SQLITE_OK) { return NULL; }
    if (records.size() == 0) { return  NULL; }
    Json::Value location;
    if (records[0][4] == "NULL") {
        location["latitude"] = "NULL";
    }
    else {
        location["latitude"] = stof(records[0][4]);
    }

    if (records[0][5] == "NULL") {
        location["longitude"] = "NULL";
    }
    else {
        location["longitude"] = stof(records[0][5]);
    }
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
        data["rating"] = stof(records[0][2]);
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
    return data;
}

std::vector<std::string> Database::split(std::string str, std::string token) {
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
