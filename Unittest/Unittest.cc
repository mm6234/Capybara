// NOTES
// UNITTEST MUST BE C++17 to run filesystem
//
#include "../Capybara/intermediary.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <json/json.h>
#include <tuple>
#include <filesystem>           // Test if a File Exists
#include <iostream>
#include <sqlite3.h>
#include "../Capybara/databaseAbstract.h"
#include "../Capybara/database.h"

using ::testing::Return;

class MockJsonValue {
public:
	Json::Value location;
	Json::Value other;
	Json::Value data;
    string dataString;
	MockJsonValue() {
        location["latitude"] = 3.222;
        location["longitude"] = 78.43;
        other["streetAddress"] = "NYC";

        data["id"] = 3;
        data["doctorName"] = "Capybara";
        data["rating"] = 3.5;
        data["ratingSubmissions"] = 99;
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

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        dataString = Json::writeString(builder, data);
	}
    nlohmann::json updateParsonJsonWithAllFields = nlohmann::json::parse(R"(
    {
        "id": 3,
        "fieldToUpdate": "doctorName",
        "fieldValue": "Capybara"
    } 
    )");
    nlohmann::json updateParsonJsonWithoutFieldToUpdate = nlohmann::json::parse(R"(
    {
        "id": 3,
        "fieldValue": "Capybara"
    } 
    )");
    nlohmann::json updateParsonJsonWithoutId = nlohmann::json::parse(R"(
    {
        "fieldToUpdate": "doctorName",
        "fieldValue": "Capybara"
    } 
    )");
    nlohmann::json updateParsonJsonWithFieldToUpdate = nlohmann::json::parse(R"(
    {
        "fieldToUpdate": "doctorName"
    } 
    )");

};

class MockIntermediary : public DatabaseAbstract {
public:
    MOCK_METHOD((Json::Value), getDataById, (const string id), (override));
    MOCK_METHOD(int, updateDoctorDatabase, (string doctorId, string& fieldToUpdate, string& fieldValue), (override));
    MOCK_METHOD(int, updateCreateNewRecord, (const string& fieldToUpdate, const string& fieldValue), (override));
    MOCK_METHOD((vector <string>), split, (string str, string token), (override));
    MOCK_METHOD((Json::Value), getDataByQuery, (string query), (override));
};

TEST(Constructor, preexistingDB) {
    MockJsonValue data;
    MockIntermediary m;
    
    std::filesystem::path fpath = "db.db";
    EXPECT_TRUE(std::filesystem::exists(fpath));

    // NEED TO CHECK IF this->db already has existing database
}

//TEST(Constructor, newDB) {
//    MockJsonValue m;
//    Intermediary a;
//
//    std::filesystem::path fpath = "db.db";
//    EXPECT_TRUE(std::filesystem::exists(fpath));
//}


//TEST(DoctorInfo, getDataByIdSuccess) {
//    MockJsonValue m;
//    Intermediary a;
//
//    Json::StreamWriterBuilder builder;
//    builder["indentation"] = "";
//    string dataString = Json::writeString(builder, m.data);
//    tuple <int, string> m_tup =  make_tuple(200, dataString);
//
//    EXPECT_EQ(a.doctorInfo("1"), m_tup);
//}

//TEST(DoctorInfo, getDataByIdFailure) {
//    MockJsonValue m;
//    EXPECT_EQ(getDataById("s"), NULL);
//}

TEST(DoctorInfo, doctorInfoSuccess200) {

    MockJsonValue data;
    MockIntermediary m;
    EXPECT_CALL(m, getDataById("1"))
        .WillRepeatedly(Return(data.data));
    
    Intermediary i(&m);
    tuple <int, string> result = i.doctorInfo("1");

    EXPECT_EQ(get<0>(result), 200);
    EXPECT_EQ(get<1>(result), data.dataString);
}

TEST(DoctorInfo, doctorInfoFailure400) {
    MockJsonValue data;
    MockIntermediary m;
    EXPECT_CALL(m, getDataById("s"))
        .WillRepeatedly(Return(NULL));

    Intermediary i(&m);
    tuple <int, string> result = i.doctorInfo("s");

    EXPECT_EQ(get<0>(result), 400);
    EXPECT_EQ(get<1>(result), "{\"error\": \"Illegal 'id' field!\"}");
}

TEST(Update, updateUpdateDatabaseSuccess200) {

    MockJsonValue data;
    MockIntermediary m;
    string id = data.data["id"].asString();
    string fieldToUpdate = "doctorName";
    string fieldValue = data.data[fieldToUpdate].asString();

    EXPECT_CALL(m, updateDoctorDatabase(id, fieldToUpdate, fieldValue))
        .WillRepeatedly(Return(0));

    Intermediary i(&m);
	tuple<int, string> result = i.update(data.updateParsonJsonWithAllFields);
	
    EXPECT_EQ(get<0>(result), 200);
    EXPECT_EQ(get<1>(result), "");
}

TEST(Update, updateUpdateDatabaseFailure200) {
    MockJsonValue data;
    MockIntermediary m;
    Intermediary i(&m);

    std::tuple<int, string> result = i.update(data.updateParsonJsonWithoutFieldToUpdate);

    EXPECT_EQ(get<0>(result), 400);
    EXPECT_EQ(get<1>(result), "{\"error\": \"Invalid JSON format in the request body\"}");
}

TEST(Update, updateCreateDatabaseSuccess200) {
    MockJsonValue data;
    MockIntermediary m;
    int id = stoi(data.data["id"].asString());
    string fieldToUpdate = "doctorName";
    string fieldValue = data.data[fieldToUpdate].asString();

    EXPECT_CALL(m, updateCreateNewRecord(fieldToUpdate, fieldValue))
        .WillRepeatedly(Return(id));

    Intermediary i(&m);
    std::tuple<int, string> result = i.update(data.updateParsonJsonWithoutId);

    EXPECT_EQ(get<0>(result), 200);
    EXPECT_EQ(get<1>(result), "{\"id\": 3}");
}

TEST(Update, updateCreateDatabaseFailure400) {
    MockJsonValue data;
    MockIntermediary m;

    Intermediary i(&m);
    std::tuple<int, string> result = i.update(data.updateParsonJsonWithFieldToUpdate);

    EXPECT_EQ(get<0>(result), 400);
    EXPECT_EQ(get<1>(result), "{\"error\": \"Invalid JSON format in the request body\"}");
}

TEST(Query, queryRatingSuccess200) {
    MockJsonValue data;
    MockIntermediary m;
    string query = "select * from doctorInfo order by rating desc;";

    EXPECT_CALL(m, getDataByQuery(query))
        .WillRepeatedly(Return(data.data));

    Intermediary i(&m);
    std::tuple<int, string> result = i.query("rating", "Yes");

    EXPECT_EQ(get<0>(result), 200);
    EXPECT_EQ(get<1>(result), data.dataString);
}

TEST(Query, queryRatingSubmissionsSuccess200) {
    MockJsonValue data;
    MockIntermediary m;
    string query = "select * from doctorInfo order by ratingSubmissions desc;";

    EXPECT_CALL(m, getDataByQuery(query))
        .WillRepeatedly(Return(data.data));

    Intermediary i(&m);
    std::tuple<int, string> result = i.query("ratingSubmissions", "Yes");

    EXPECT_EQ(get<0>(result), 200);
    EXPECT_EQ(get<1>(result), data.dataString);
}

TEST(Query, queryLocationSuccess200) {
    MockJsonValue data;
    MockIntermediary m;
    string query = "select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-50.1)*(latitude-50.1)+(longitude-50.1)*(longitude-50.1) as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;";

    vector<string> coordinates{ "50.1", "50.1" };
    EXPECT_CALL(m, getDataByQuery(query))
        .WillRepeatedly(Return(data.data));
    EXPECT_CALL(m, split("50.1_50.1", "_"))
        .WillRepeatedly(Return(coordinates));

    Intermediary i(&m);
    std::tuple<int, string> result = i.query("location", "50.1_50.1");

    EXPECT_EQ(get<0>(result), 200);
    EXPECT_EQ(get<1>(result), data.dataString);
}

TEST(Database, databaseTest) {
    // Test all functions that read/write to database here
    char* error;
    sqlite3* db;
    int opened = sqlite3_open("db.db", &db);
    string query1 = "DELETE FROM doctorInfo;";
    int exec1 = sqlite3_exec(db, query1.c_str(), NULL, NULL, &error);
    EXPECT_EQ(exec1, SQLITE_OK);
    
    Database database;
    MockJsonValue data;

    auto result1 = database.getDataById("1");
    EXPECT_EQ(result1, NULL);

    auto result2 = database.getDataById("s");
    EXPECT_EQ(result2, NULL);

    auto result3 = database.getDataByQuery("");
    EXPECT_EQ(result3, NULL);

    auto result4 = database.getDataByQuery("select * from doctorInfo order by rating desc;");
    EXPECT_EQ(result4, NULL);

    auto result5 = database.getDataByQuery("select * from doctorInfo order by ratingSubmissions desc;");
    EXPECT_EQ(result5, NULL);

    auto result6 = database.getDataByQuery("select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-50.1)*(latitude-50.1)+(longitude-50.1)*(longitude-50.1) as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;");
    EXPECT_EQ(result6, NULL);

    vector<string> fieldToUpdate = { "doctorName", "rating", "ratingSubmissions", \
                                                        "latitude", "longitude"};
    vector<vector<string>> fieldValue{ { "Capybara1", "Capybara2"}, \
                                                            {"3.5", "4.8"}, \
                                                            { "18000", "10" }, \
                                                            {"10.1", "100.1"}, \
                                                            {"10.1", "100.1"} };

    auto result7 = database.updateCreateNewRecord(fieldToUpdate[0], fieldValue[0][0]);
    EXPECT_EQ(result7, 1);

    auto result8 = database.updateDoctorDatabase("1", fieldToUpdate[1], fieldValue[1][0]);
    EXPECT_EQ(result8, SQLITE_OK);

    auto result9 = database.updateDoctorDatabase("1", fieldToUpdate[2], fieldValue[2][0]);
    EXPECT_EQ(result9, SQLITE_OK);

    auto result10 = database.updateDoctorDatabase("1", fieldToUpdate[3], fieldValue[3][0]);
    EXPECT_EQ(result10, SQLITE_OK);

    auto result11 = database.updateDoctorDatabase("1", fieldToUpdate[4], fieldValue[4][0]);
    EXPECT_EQ(result11, SQLITE_OK);

    auto result12 = database.updateCreateNewRecord(fieldToUpdate[0], fieldValue[0][1]);
    EXPECT_EQ(result12, 2);

    auto result13 = database.updateDoctorDatabase("2", fieldToUpdate[1], fieldValue[1][1]);
    EXPECT_EQ(result13, SQLITE_OK);

    auto result14 = database.updateDoctorDatabase("2", fieldToUpdate[2], fieldValue[2][1]);
    EXPECT_EQ(result14, SQLITE_OK);

    auto result15 = database.updateDoctorDatabase("2", fieldToUpdate[3], fieldValue[3][1]);
    EXPECT_EQ(result15, SQLITE_OK);

    auto result16 = database.updateDoctorDatabase("2", fieldToUpdate[4], fieldValue[4][1]);
    EXPECT_EQ(result16, SQLITE_OK);

    auto result17 = database.getDataById("1");
    EXPECT_EQ(result17[fieldToUpdate[0]], fieldValue[0][0]);
    EXPECT_EQ(result17[fieldToUpdate[2]], stoi(fieldValue[2][0]));

    auto result18 = database.getDataById("2");
    EXPECT_EQ(result18[fieldToUpdate[1]], stof(fieldValue[1][1]));
    EXPECT_EQ(result18["location"][fieldToUpdate[3]], stof(fieldValue[3][1]));

    auto result19 = database.getDataByQuery("select * from doctorInfo order by rating desc;");
    EXPECT_EQ(result19["id"], 2);
    EXPECT_EQ(result19[fieldToUpdate[1]], stof(fieldValue[1][1]));

    auto result20 = database.getDataByQuery("select * from doctorInfo order by ratingSubmissions desc;");
    EXPECT_EQ(result20["id"], 1);
    EXPECT_EQ(result20[fieldToUpdate[2]], stoi(fieldValue[2][0]));

    auto result21 = database.getDataByQuery("select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-80.1)*(latitude-80.1)+(longitude-80.1)*(longitude-80.1) as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;");
    EXPECT_EQ(result21["id"], 2);
    EXPECT_EQ(result21["location"][fieldToUpdate[3]], stof(fieldValue[3][1]));
    EXPECT_EQ(result21["location"][fieldToUpdate[4]], stof(fieldValue[4][1]));
}