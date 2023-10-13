
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
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;

    try {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    // Selects the Doctor Rankings Database
    con->setSchema("coms-w4156");

    // Example Execute Statement, Retrieves SQL Version
    stmt = con->createStatement();
    stmt->execute("SELECT @@version");
    std::cout << stmt->getResultSet();

    // Example Execute Statement with Existing Values
    stmt = con->createStatement();
    stmt->execute("DROP TABLE IF EXISTS inventory");
    cout << "Finished dropping table (if existed)" << endl;
    stmt->execute("INSERT INTO coms-w4156 (id, doctorName, rating, location, keywords, languagesSpoken, insurancesAccepted, additionalDetails) VALUES ('1', 'name', '5.0', '(50.000, 70.000)', '(ears, nose, throat)', 'English', '(insurance1, insurance2)', '(Street Address)';");
    cout << "Table Created." << endl;
    delete stmt;

    // Inputting Prepared SQL Statements
    pstmt = con->prepareStatement("INSERT INTO coms-w4156(doctorName, rating) VALUES(?,?)");
    pstmt->setString(1, "name");
    pstmt->setFloat(2, 5.0);
    pstmt->execute();
    cout << "Successfully Inserted" << endl;

    //delete pstmt;
    delete con;
    system("pause");
    return 0;
}
