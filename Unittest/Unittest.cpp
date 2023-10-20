#include <gtest/gtest.h>
#include "..\\Capybara\\Capybara.h"
#include <drogon/drogon.h>

namespace {
    TEST(UnitTest, doctorInfo) {
        Json::Value location;
        location["latitude"] = 3.222;
        location["longitude"] = 78.43;
        Json::Value other;
        other["streetAddress"] = "NYC";

        Json::Value mockDoctorInfo;
        mockDoctorInfo["id"] = 3;
        mockDoctorInfo["doctorName"] = "Capybara";
        mockDoctorInfo["rating"] = 3.5;
        mockDoctorInfo["location"] = location;
        mockDoctorInfo["practiceKeywords"] = Json::arrayValue;
        mockDoctorInfo["practiceKeywords"].append("Ear");
        mockDoctorInfo["practiceKeywords"].append("Nose");
        mockDoctorInfo["practiceKeywords"].append("Throat");
        mockDoctorInfo["languagesSpoken"] = Json::arrayValue;
        mockDoctorInfo["languagesSpoken"].append("English");
        mockDoctorInfo["insurance"] = Json::arrayValue;
        mockDoctorInfo["languagesSpoken"].append("Aetna");
        mockDoctorInfo["other"] = other;
        Capybara c;
        EXPECT_EQ(c.getDataById("1"), mockDoctorInfo);
    }
    TEST(UnitTest, Update) {
        Json::Value mockUpdateInfo;
        mockUpdateInfo["id"] = 3;
        mockUpdateInfo["fieldtoUpdate"] = "doctorName";
        mockUpdateInfo["fieldValue"] = "Capybara Elite";

        Json::Value mockDoctorInfo;
        mockDoctorInfo["id"] = 3;
        mockDoctorInfo["doctorName"] = "Capybara Elite";
        mockDoctorInfo["rating"] = 3.5;
        mockDoctorInfo["location"] = location;
        mockDoctorInfo["practiceKeywords"] = Json::arrayValue;
        mockDoctorInfo["practiceKeywords"].append("Ear");
        mockDoctorInfo["practiceKeywords"].append("Nose");
        mockDoctorInfo["practiceKeywords"].append("Throat");
        mockDoctorInfo["languagesSpoken"] = Json::arrayValue;
        mockDoctorInfo["languagesSpoken"].append("English");
        mockDoctorInfo["insurance"] = Json::arrayValue;
        mockDoctorInfo["insurance"].append("Aetna");
        mockDoctorInfo["other"] = other;
        Capybara c;
        Json::Value origdoctorInfo = c.doctorDatabase[3];
        EXPECT_EQ(origdoctorInfo, mockDoctorInfo);
    }
}
