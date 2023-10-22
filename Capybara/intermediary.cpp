#include "intermediary.h"

Json::Value getDataById(const string id) {
    //TODO: Sql query to get all values
    int id_int;
    try {
        id_int = atoi(id.c_str());
        if (id_int > 1 || id_int <= 0) {             // If out of bounds, returns NULL
            return NULL;
        }
    }
    catch (const std::exception& e){
        return NULL;
    }


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


std::unordered_map<int, Json::Value> updateDoctorDatabase(int doctorId, const std::string& fieldToUpdate, const std::string& fieldValue) {
    std::unordered_map<int, Json::Value> doctorDatabase;
    if (doctorDatabase.find(doctorId) != doctorDatabase.end()) {
        doctorDatabase[doctorId][fieldToUpdate] = Json::Value(fieldValue);
    }
    else {
        Json::Value newDoctor;
        newDoctor[fieldToUpdate] = Json::Value(fieldValue);
        doctorDatabase[doctorId] = newDoctor;
    }
    return doctorDatabase;
}