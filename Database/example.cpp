#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

using namespace std;

const string server = "";
const string username = "";
const string password = "";

int main()
{
    try {
        sql::Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::ResultSet* res;
        sql::PreparedStatement* pstmt;

        // Setup Connection & Choose Database
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
        con->setSchema("defaultdb");

        // Execute Example Statement with Expected Output
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT VERSION()");   
        while (res->next()) {
            cout << "MySQL Database Version: " << res->getString(1) << endl;
        }

        // Execute Create Table Row Entry Command
        stmt = con->createStatement();
        res = stmt->executeQuery("INSERT INTO defaultdb(lastName, firstName, ranking, numRankings, latitude, longitude, practiceFocus, languages, insurances, extraDetails) VALUES ('Smith', 'Jane', 5.0, 1, 40.8075083, -73.9626806, 'Ears;Nose;Throat', 'English;Spanish', 'insurance1;insurance2', '116th and Broadway, NY, NY 10027')");

        // Execute Prepared Statement for Creating Table Row
        pstmt = con->prepareStatement("INSERT INTO defaultdb(lastName, firstName, ranking, numRankings, latitude, longitude, practiceFocus, languages, insurances, extraDetails) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        pstmt->setString(1, "Smith");
        pstmt->setString(2, "Jane");
        pstmt->setDouble(3, 5.0);
        pstmt->setInt(4, 1);
        pstmt->setDouble(5, 40.8075083);
        pstmt->setDouble(6, -73.9626806);
        pstmt->setString(7, "Ears;Nose;Throat");
        pstmt->setString(8, "English;Spanish");
        pstmt->setString(9, "insurance1;insurance2");
        pstmt->setString(10, "116th and Broadway, NY, NY 10027");
        pstmt->execute();
        
        // Execute Select Row/Value from Table Command
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM doctors");
        while (res->next()) {
            cout << "Name = " << res->getString("firstName") << " " << res->getString("lastName");
            cout << "Ranking = " << res->getDouble("ranking");
        }

        // Execute Update Table Entry Command
        pstmt = con->prepareStatement("UPDATE doctors SET ranking = ? WHERE lastName = ? AND firstName = ?");
        pstmt->setDouble(1, 5.0);
        pstmt->setString(2, "Smith");
        pstmt->setString(3, "Jane");
        pstmt->execute();

        // Searching for Keywords Stored in Table (Returning Entries)
        string keyword = "keyword";
        pstmt = con->prepareStatement("SELECT * FROM doctors WHERE languages LIKE ?");
        pstmt->setString(1, "%" + keyword + "%");
        res = pstmt->executeQuery();
        while (res->next()) {
            cout << "Name = " << res->getString("firstName") << " " << res->getString("lastName");
            cout << "Ranking = " << res->getDouble("ranking");
        }

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "# ERR: SQL Exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    
    return 0;
}