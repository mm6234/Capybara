# Capybara

## Install MySQL Server

Visit the [MySQL Downloads page](mysql.com/downloads)
* Go to MySQL Community (GPL) Downloads
* MySQL Installer for Windows
* Download the larger file of the two shown (i.e. mysql-installer-community-8.0.27.1.msi)

Open the MySQL Installer file
* Click 'Custom' install
* Include both MySQL Server _(MySQL Servers->MySQL Server 8.0->MySQL Server 8.0.27 -X64)_ and MySQL Client _(Applications->MySQL Workbench->MySQL Workbench 8.0->MySQL Workbench 8.0.27 – X64)_
* Click ‘Next’, then ‘Execute’, then ‘Next’, ‘Next’, ‘Next’, ‘Next’, choose a Root password, ‘Next’, ‘Next’, ‘Execute’, ‘Finish’, ‘Next’, ‘Finish’

## Linking System Environment Variable to MySQL Library

1. Right click on the bottom left Windows icon
2. Choose 'System'
3. Select 'Advanced System Settings'
4. Click 'Environment Variables'
5. Within the bottom 'System Variables' listing, select the 'Path' variable and press 'Edit...'
6. At the very beginning of the field within the popup window, add the MySQL Server directory (i.e. %ProgramFiles%\MySQL\MySQL Server 8.0\bin) and append a semicolon
7. Click 'Ok' on every window to exit

## Opening the Database in MySQL Workbench

Open the MySQL Workbench program
* Select the '+' button to add a new MySQL Connection
* Enter the database credentials provided in the Discord server

## Setting up the Visual Studio Environment

Download the Windows (x86, 64-bit) ZIP archive from the [MySQL downloads page](https://dev.mysql.com/downloads/connector/cpp/)
* Un-zip the 'mysql-connector-c++' folder
* Open the Capybara project in Visual Studio
* Change the project Solution Configuration setting within the menu bar from 'Debug' to 'Release'
* Click the 'Project' menu item, and select 'Project' properties at the bottom of the listing
* In the popup window, ensure 'Configuration' is set to 'Release', and 'Platform' is 'x64'
* Click 'C/C++' > General > Additional Include Directories > Select the dropdown icon > Click 'Edit'
  * Select the yellow create icon in the popup window, then the '...' button that appears
  * Copy and paste the following directory in the field: [Directory you stored the Un-Zipped MySQL Connector Library]/include/jdbc
* Click 'C/C++' > Preprocessor > Preprocessor Definitions
  * Add `STATIC_CONCPP;` to the beginning of the field value
* Click 'C/C++' > Code Generation > Runtime Library
  * Select `Multi-Threaded DLL (/MD)`
* Click 'Linker' > Input > Additional Dependencies
  * Select the dropdown icon > Click 'Edit'
  * Input `mysqlcppcon-static.lib` in the white box
 
Download and Run the solution.
