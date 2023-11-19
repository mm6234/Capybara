// NOTES
// UNITTEST MUST BE C++17 to run filesystem
//
#include "../Capybara/intermediary.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <tuple>
#include <filesystem>           // Test if a File Exists
#include <iostream>
#include "../Capybara/databaseAbstract.h"

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