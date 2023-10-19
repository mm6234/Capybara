// Capybara.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#ifdef PROJECT1_EXPORTS
#define PROJECT1_API __declspec(dllexport)
#else
#define PROJECT1_API __declspec(dllimport)
#endif
#include <iostream>
#include <drogon/drogon.h>          // does not json.h i assume
#include <fstream>
#include <filesystem>          // PWD
#include <nlohmann/json.hpp>
class Capybara {
public:
	Json::Value getDataById(std::string);
};

