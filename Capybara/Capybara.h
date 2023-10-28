// Capybara.h : Capybara API utilities

#pragma once
#include <drogon/drogon.h>          // does not json.h i assume
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <iostream>
using namespace std;
using namespace drogon;


string capybanner = R"(  ,--,    .--.  ,---..-.   .-.,---.     .--.  ,---.    .--.1.0
.' .')   / /\ \ | .-.\\ \_/ )/| .-.\   / /\ \ | .-.\  / /\ \  
|  |(_) / /__\ \| |-' )\   (_)| |-' \ / /__\ \| `-'/ / /__\ \ 
\  \    |  __  || |--'  ) (   | |--. \|  __  ||   (  |  __  | 
 \  `-. | |  |)|| |     | |   | |`-' /| |  |)|| |\ \ | |  |)| 
  \____\|_|  (_)/(     /(_|   /( `--' |_|  (_)|_| \)\|_|  (_) 
               (__)   (__)   (__)                 (__)        )";

tuple <string, string> queryGetFieldFromValue(string ratingVal, string ratingSubmissionsVal, string locationVal) {
    if (ratingVal == "Yes") {
        return make_tuple("rating", ratingVal);
    }
    if (ratingSubmissionsVal == "Yes") {
        return make_tuple("ratingSubmissions", ratingSubmissionsVal);
    }
    if (locationVal != "") {
        return make_tuple("location", locationVal);
    }
    return make_tuple("", "");
}


HttpStatusCode convertStatusCode(int number) {
    // Sets status Codes
    if (number == 200) {
        return HttpStatusCode::k200OK;
    }
    else {
        return HttpStatusCode::k400BadRequest;
    }
}