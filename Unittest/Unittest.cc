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

//################################################
// Unit tests for intermediary ##########################
//################################################

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

//################################################
// Unit tests for database #############################
//################################################

TEST(DatabaseTest, databaseTest) {
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

//################################################
// Internal Integration Tests ###########################
//################################################

TEST(InternalIntegrationTest, internalIntegrationTest) {

    // The data must come after the unit tests for Database is done
    // We will need the data previously inserted 
    MockJsonValue data;
    DatabaseAbstract* database = new Database();
    Intermediary i(database);

    // add a third doctorInfo data
    tuple<int, string> result22 = i.update(data.updateParsonJsonWithoutId);
    EXPECT_EQ(get<0>(result22), 200);
    EXPECT_EQ(get<1>(result22), "{\"id\": 3}");

    tuple<int, string> result23 = i.update(data.updateParsonJsonWithFieldToUpdate);
    EXPECT_EQ(get<0>(result23), 400);
    EXPECT_EQ(get<1>(result23), "{\"error\": \"Invalid JSON format in the request body\"}");

    // Update name to Capybara
    tuple<int, string> result24 = i.update(data.updateParsonJsonWithAllFields);
    EXPECT_EQ(get<0>(result24), 200);

    tuple<int, string> result25 = i.update(data.updateParsonJsonWithoutFieldToUpdate);
    EXPECT_EQ(get<0>(result25), 400);
    EXPECT_EQ(get<1>(result25), "{\"error\": \"Invalid JSON format in the request body\"}");

    auto [statusCode26, stringBody26] = i.doctorInfo("3");
    EXPECT_EQ(statusCode26, 200);
    Json::Value result26;
    Json::Reader r26;
    bool parsed26 = r26.parse(stringBody26.c_str(), result26);
    EXPECT_EQ(parsed26, 1);
    EXPECT_EQ(result26["doctorName"], "Capybara");

    auto [statusCode27, stringBody27] = i.doctorInfo("s");
    EXPECT_EQ(statusCode27, 400);
    EXPECT_EQ(stringBody27, "{\"error\": \"Illegal 'id' field!\"}");

    auto [statusCode28, stringBody28] = i.query("rating", "Yes");
    EXPECT_EQ(statusCode28, 200);
    Json::Value result28;
    Json::Reader r28;
    bool parsed28 = r28.parse(stringBody28.c_str(), result28);
    EXPECT_EQ(parsed28, 1);
    EXPECT_EQ(result28["doctorName"], "Capybara2");

    auto [statusCode29, stringBody29] = i.query("ratingSubmissions", "Yes");
    EXPECT_EQ(statusCode29, 200);
    Json::Value result29;
    Json::Reader r29;
    bool parsed29 = r29.parse(stringBody29.c_str(), result29);
    EXPECT_EQ(parsed29, 1);
    EXPECT_EQ(result29["doctorName"], "Capybara1");

    auto [statusCode30, stringBody30] = i.query("location", "80.1_80.1");
    EXPECT_EQ(statusCode30, 200);
    Json::Value result30;
    Json::Reader r30;
    bool parsed30 = r30.parse(stringBody30.c_str(), result30);
    EXPECT_EQ(parsed30, 1);
    EXPECT_EQ(result30["doctorName"], "Capybara2");

}

//################################################
// External Integration Tests ###########################
//################################################

TEST(ExternalIntegrationTest, externalIntegrationTest) {
    // The data must come after internal integration tests is done
    // We will need the data previously inserted 
    
    // Test sqlite3
    Database* database = new Database();
    char* error;
    sqlite3* db;
    int opened = sqlite3_open("db.db", &db);

    // check length of rows
    vector<vector<string>> records31;
    string query31 = "select count(*) from doctorInfo";
    int exec31 = sqlite3_exec(db, query31.c_str(), database->select_callback, &records31, &error);
    EXPECT_EQ(exec31, SQLITE_OK);
    EXPECT_EQ(records31[0][0], "3");

    // insert new data
    vector<string> values(10, "NULL, ");
    values[0] = "4, ";
    string query32 = "insert into doctorInfo VALUES (";
    for (const auto& e : values) query32 += e;
    query32.pop_back();
    query32.pop_back();
    query32 += ");";
    int exec32 = sqlite3_exec(db, query32.c_str(), NULL, NULL, &error);
    EXPECT_EQ(exec32, SQLITE_OK);

    // update the new data
    string query33 = "update doctorInfo set doctorName = \"externalTest\" where id = 4; ";
    int exec33 = sqlite3_exec(db, query33.c_str(), NULL, NULL, &error);
    EXPECT_EQ(exec33, SQLITE_OK);

    // get new data
    vector<vector<string>> records34;
    string query34 = "select * from doctorInfo where id=4 and id is not null;";
    int exec34 = sqlite3_exec(db, query34.c_str(), database->select_callback , &records34, &error);
    EXPECT_EQ(exec34, SQLITE_OK);
    EXPECT_EQ(records34[0][0], "4");
    EXPECT_EQ(records34[0][1], "externalTest");

    // query rating
    vector<vector<string>> records35;
    string query35 = "select * from doctorInfo order by rating desc;";
    int exec35 = sqlite3_exec(db, query35.c_str(), database->select_callback, &records35, &error);
    EXPECT_EQ(exec35, SQLITE_OK);
    EXPECT_EQ(records35[0][0], "2");
    EXPECT_EQ(records35[0][1], "Capybara2");

    // query ratingSubmissions
    vector<vector<string>> records36;
    string query36 = "select * from doctorInfo order by ratingSubmissions desc;";
    int exec36 = sqlite3_exec(db, query36.c_str(), database->select_callback, &records36, &error);
    EXPECT_EQ(exec36, SQLITE_OK);
    EXPECT_EQ(records36[0][0], "1");
    EXPECT_EQ(records36[0][1], "Capybara1");

    // query location
    vector<vector<string>> records37;
    string query37 = "select id, doctorName, rating, ratingSubmissions, \
latitude, longitude, practiceKeywords, languagesSpoken, \
insurance, streetAddress, \
(latitude-80.1)*(latitude-80.1)+(longitude-80.1)*(longitude-80.1) as diff \
from doctorInfo where latitude is not NULL and longitude is not NULL order by diff asc;";
    int exec37 = sqlite3_exec(db, query37.c_str(), database->select_callback, &records37, &error);
    EXPECT_EQ(exec37, SQLITE_OK);
    EXPECT_EQ(records37[0][0], "2");
    EXPECT_EQ(records37[0][1], "Capybara2");

    // Test nlohmann::json
    nlohmann::json updateParsonJsonWithAllFields = nlohmann::json::parse(R"(
    {
        "id": 3,
        "fieldToUpdate": "doctorName",
        "fieldValue": "Capybara"
    } 
    )");

    EXPECT_TRUE(updateParsonJsonWithAllFields.find("id") != updateParsonJsonWithAllFields.end());
    EXPECT_EQ(updateParsonJsonWithAllFields["id"].get<int>(), 3);
    EXPECT_TRUE(updateParsonJsonWithAllFields.find("fieldToUpdate") != updateParsonJsonWithAllFields.end());
    EXPECT_EQ(updateParsonJsonWithAllFields["fieldToUpdate"].get<string>(), "doctorName");
    EXPECT_TRUE(updateParsonJsonWithAllFields.find("fieldValue") != updateParsonJsonWithAllFields.end());
    EXPECT_EQ(updateParsonJsonWithAllFields["fieldValue"].get<string>(), "Capybara");

}

TEST(AdditionalFieldTest, additionalFieldTest) {

    // In order to test out different fields when inserting/converting data, we will test out the remaining 8 fields
    // for distinction, all results start with "21"

    Database database;
    MockJsonValue data;

    auto result211 = database.updateCreateNewRecord("rating", "10.1");
    EXPECT_EQ(result211, 5);
    auto result212 = database.getDataById("5");
    EXPECT_EQ(result212["rating"], stof("10.1"));

    auto result213 = database.updateCreateNewRecord("ratingSubmissions", "100000");
    EXPECT_EQ(result213, 6);
    auto result214 = database.getDataById("6");
    EXPECT_EQ(result214["ratingSubmissions"], stoi("100000"));

    auto result215 = database.updateCreateNewRecord("latitude", "1.1");
    EXPECT_EQ(result215, 7);
    auto result216 = database.getDataById("7");
    EXPECT_EQ(result216["location"]["latitude"], stof("1.1"));

    auto result217 = database.updateCreateNewRecord("longitude", "1.1");
    EXPECT_EQ(result217, 8);
    auto result218 = database.getDataById("8");
    EXPECT_EQ(result218["location"]["longitude"], stof("1.1"));

    auto result219 = database.updateCreateNewRecord("practiceKeywords", "nose");
    EXPECT_EQ(result219, 9);
    auto result2110 = database.getDataById("9");
    EXPECT_EQ(result2110["practiceKeywords"], "nose");

    auto result2111 = database.updateCreateNewRecord("languagesSpoken", "english");
    EXPECT_EQ(result2111, 10);
    auto result2112 = database.getDataById("10");
    EXPECT_EQ(result2112["languagesSpoken"], "english");

    auto result2113 = database.updateCreateNewRecord("insurance", "aetna");
    EXPECT_EQ(result2113, 11);
    auto result2114 = database.getDataById("11");
    EXPECT_EQ(result2114["insurance"], "aetna");

    auto result2115 = database.updateCreateNewRecord("streetAddress", "NYC");
    EXPECT_EQ(result2115, 12);
    auto result2116 = database.getDataById("12");
    EXPECT_EQ(result2116["other"]["streetAddress"], "NYC");

    auto result2117 = database.getDataByQuery("select * from doctorInfo order by rating desc;");
    EXPECT_EQ(result2117["id"], stoi("5"));
    EXPECT_EQ(result2117["rating"], stof("10.1"));

    auto result2118 = database.getDataByQuery("select * from doctorInfo order by ratingSubmissions desc;");
    EXPECT_EQ(result2118["id"], stoi("6"));
    EXPECT_EQ(result2118["ratingSubmissions"], stoi("100000"));

}