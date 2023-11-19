#pragma once		
#include <json/json.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <stdio.h>
#include <tuple>
#include <iostream>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "databaseAbstract.h"

class Database : public DatabaseAbstract {
public:
	sqlite3* db;
	using Record = std::vector<std::string>;
	using Records = std::vector<Record>;

	Database() {
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
                            streetAddress varchar(255) \
                            );", NULL, NULL, &error);
		if (rc != SQLITE_OK) {
			cout << "[-] Error Initializing Database!" << endl;
			cerr << error << endl;
		}
	}

	~Database() override {
		sqlite3_close(this->db);
		cout << "[+] Successfully Closed Database!" << endl;
	}

	Json::Value getDataById(const std::string id) override;

	static int select_callback(void* ptr, int argc, char* argv[], char* cols[])
	// https://stackoverflow.com/questions/15836253/c-populate-vector-from-sqlite3-callback-function
	{
		Records* table = static_cast<Records*>(ptr);
		vector<string> row;
		for (int i = 0; i < argc; i++)
			row.push_back(argv[i] ? argv[i] : "NULL");
		table->push_back(row);
		return 0;
	}
	int updateDoctorDatabase(std::string doctorId, std::string& fieldToUpdate, std::string& fieldValue) override;

	int updateCreateNewRecord(const std::string& fieldToUpdate, const std::string& fieldValue) override;

	std::vector<std::string> split(std::string str, std::string token) override;
};