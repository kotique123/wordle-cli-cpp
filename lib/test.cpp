#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

using namespace std;

// Function that returns table names as a string
string getTables(SQLite::Database& db) {
    string tables = "";
    SQLite::Statement query(db, "SELECT name FROM sqlite_master WHERE type='table'");
    
    while (query.executeStep()) {
        if (!tables.empty()) {
            tables += ", ";
        }
        tables += query.getColumn(0).getString();
    }
    
    return tables;
}

// Try multiple possible database paths
SQLite::Database* openDatabase() {
    vector<string> possiblePaths = {
        "res/db/words5.db",
        "../res/db/words5.db",
        "../../res/db/words5.db",
        "bin/res/db/words5.db"
    };
    
    for (const string& path : possiblePaths) {
        try {
            cout << "Trying to open database at: " << path << endl;
            return new SQLite::Database(path);
        } catch (const SQLite::Exception& e) {
            cout << "Failed to open " << path << ": " << e.what() << endl;
        }
    }
    
    throw runtime_error("Could not open database file from any expected location");
}

int main() {
    try {
        SQLite::Database* db = openDatabase();
        
        // Test the getTables function
        string tableNames = getTables(*db);
        cout << "Tables in database: " << tableNames << endl;
        
        // Test querying the words table
        SQLite::Statement query(*db, "SELECT COUNT(*) FROM words");
        if (query.executeStep()) {
            cout << "Number of words in database: " << query.getColumn(0).getInt() << endl;
        }
        
        delete db;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}