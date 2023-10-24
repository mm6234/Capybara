#include "../Capybara/intermediary.h"
#include <gtest/gtest.h>


TEST(TEST1, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

TEST(JSON_test, Test_getDataById) {
	Json::Value test = getDataById("1");
	Json::StreamWriterBuilder writer;
	std::string json_str = Json::writeString(writer, test);

	EXPECT_EQ(json_str, "string test");
}