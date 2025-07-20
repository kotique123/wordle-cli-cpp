/* How the game process has to go:
 * 0. You convert guess word into a vector of type Letter. Pre-initialize everything
 *    to false.
 * 1. You compare character by their indexes. If index is the same.
 * 2. You compare original word with guess.
 * 3. You compare guess to original.
 *  - If index is the same, guess word attribute correct is set to correct.
 *    and you exit the loop. 
 *  - If index is not the same, guess word attribute correct is set to wrongPosition.
 *    and you exit the loop.
 *  - If there is no letter at all - do nothing
 *
*/

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include "words.hpp"

using namespace std;

// Database loading functions
SQLite::Database* openDatabase(int wordLength) {
    string dbFileName = "words" + to_string(wordLength) + ".db";
    vector<string> possiblePaths = {
        "res/db/" + dbFileName,
        "../res/db/" + dbFileName,
        "../../res/db/" + dbFileName,
        "bin/res/db/" + dbFileName
    };
    
    for (const string& path : possiblePaths) {
        try {
            return new SQLite::Database(path, SQLite::OPEN_READWRITE);
        } catch (const SQLite::Exception& e) {
            cout << "Failed to open " << path << ": " << e.what() << endl;
        }
    }
    
    throw runtime_error("Could not open database file 'words" + to_string(wordLength) + ".db' from any expected location");
}

SQLite::Database* openDatabaseForWriting(int wordLength) {
    string dbFileName = "words" + to_string(wordLength) + ".db";
    vector<string> possiblePaths = {
        "res/db/" + dbFileName,
        "../res/db/" + dbFileName,
        "../../res/db/" + dbFileName,
        "bin/res/db/" + dbFileName
    };
    
    for (const string& path : possiblePaths) {
        try {
            // Try to open in read-write mode, create if doesn't exist
            return new SQLite::Database(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        } catch (const SQLite::Exception& e) {
            cout << "Failed to open " << path << " for writing: " << e.what() << endl;
        }
    }
    
    // If all paths fail, try to create in the first available directory
    try {
        string dbPath = "res/db/" + dbFileName;
        // Create directory if it doesn't exist
        filesystem::create_directories("res/db");
        return new SQLite::Database(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    } catch (const SQLite::Exception& e) {
        throw runtime_error("Could not create or open database file 'words" + to_string(wordLength) + ".db' for writing: " + e.what());
    }
}

void closeDatabase(SQLite::Database* db) {
    if (db) {
        delete db;
    }
}

// Custom word management functions
vector<string> parseCommaSeparatedWords(const string& input) {
    vector<string> words;
    stringstream ss(input);
    string word;
    
    while (getline(ss, word, ',')) {
        // Trim whitespace from beginning and end
        word.erase(0, word.find_first_not_of(" \t\r\n"));
        word.erase(word.find_last_not_of(" \t\r\n") + 1);
        
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    
    return words;
}

bool addCustomWord(SQLite::Database& db, const string& word) {
    try {
        SQLite::Statement query(db, "INSERT OR IGNORE INTO words (word) VALUES (?)");
        query.bind(1, word);
        query.exec();
        return true;
    } catch (const SQLite::Exception& e) {
        cout << "Error adding custom word '" << word << "': " << e.what() << endl;
        return false;
    }
}

bool addCustomWords(SQLite::Database& db, const vector<string>& words) {
    bool allSuccess = true;
    for (const string& word : words) {
        if (!addCustomWord(db, word)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool createCustomDatabase(const vector<string>& words) {
    try {
        // Group words by length
        map<int, vector<string>> wordsByLength;
        for (const string& word : words) {
            if (!word.empty()) {
                wordsByLength[word.length()].push_back(word);
            }
        }
        
        bool allSuccess = true;
        
        // Add words to existing databases based on their length
        for (const auto& pair : wordsByLength) {
            int wordLength = pair.first;
            const vector<string>& lengthWords = pair.second;
            
            try {
                // Open existing database for this word length in write mode
                SQLite::Database* db = openDatabaseForWriting(wordLength);
                
                // Ensure the words table exists
                db->exec("CREATE TABLE IF NOT EXISTS words (id INTEGER PRIMARY KEY AUTOINCREMENT, word TEXT UNIQUE)");
                
                // Add words to the existing database
                for (const string& word : lengthWords) {
                    if (!addCustomWord(*db, word)) {
                        allSuccess = false;
                    }
                }
                
                cout << "Added " << lengthWords.size() << " custom words to words" << wordLength << ".db" << endl;
                
                closeDatabase(db);
            } catch (const exception& e) {
                cout << "Could not add " << lengthWords.size() << " words to words" << wordLength << ".db: " << e.what() << endl;
                allSuccess = false;
            }
        }
        
        return allSuccess;
    } catch (const SQLite::Exception& e) {
        cout << "Error creating custom database: " << e.what() << endl;
        return false;
    }
}

bool loadCustomDatabase(const string& dbPath) {
    try {
        SQLite::Database db(dbPath);
        int wordCount = getLength(db);
        cout << "Custom database loaded: " << dbPath << " (" << wordCount << " words)" << endl;
        return true;
    } catch (const SQLite::Exception& e) {
        cout << "Error loading custom database: " << e.what() << endl;
        return false;
    }
}

vector<string> getCustomWords(SQLite::Database& db) {
    vector<string> customWords;
    try {
        SQLite::Statement query(db, "SELECT word FROM words WHERE word LIKE 'custom_%' OR word LIKE 'user_%'");
        while (query.executeStep()) {
            customWords.push_back(query.getColumn(0).getString());
        }
    } catch (const SQLite::Exception& e) {
        cout << "Error getting custom words: " << e.what() << endl;
    }
    return customWords;
}

bool removeCustomWord(SQLite::Database& db, const string& word) {
    try {
        SQLite::Statement query(db, "DELETE FROM words WHERE word = ?");
        query.bind(1, word);
        query.exec();
        return true;
    } catch (const SQLite::Exception& e) {
        cout << "Error removing custom word '" << word << "': " << e.what() << endl;
        return false;
    }
}

bool clearCustomWords(SQLite::Database& db) {
    try {
        db.exec("DELETE FROM words WHERE word LIKE 'custom_%' OR word LIKE 'user_%'");
        return true;
    } catch (const SQLite::Exception& e) {
        cout << "Error clearing custom words: " << e.what() << endl;
        return false;
    }
}

// Check if word length matches expected length
bool checkLength(int length, const string& word) {
    return word.length() == length;
}

// Get the number of rows in the words table
int getLength(SQLite::Database& db) {
    SQLite::Statement query(db, "SELECT COUNT(*) FROM words");
    if (query.executeStep()) {
        return query.getColumn(0).getInt();
    }
    return 0;
}

// Get a word at the specified index (0-based)
string getWord(SQLite::Database& db, int index) {
    SQLite::Statement query(db, "SELECT word FROM words WHERE id = ?");
    query.bind(1, index + 1); // SQLite uses 1-based indexing, so add 1
    
    if (query.executeStep()) {
        return query.getColumn(0).getString();
    }
    return ""; // Return empty string if index not found
}

// Convert a word string into a vector of characters
vector<char> getVector(const string& word) {
    return vector<char>(word.begin(), word.end());
}

// Check if a word exists in the database
bool isWordInDatabase(SQLite::Database& db, const string& word) {
    SQLite::Statement query(db, "SELECT COUNT(*) FROM words WHERE word = ?");
    query.bind(1, word);
    
    if (query.executeStep()) {
        return query.getColumn(0).getInt() > 0;
    }
    return false;
}

// Create a vector of Letter structs from a word
vector<Letter> createLetterVector(const vector<char>& word) {
    vector<Letter> letters;
    letters.reserve(word.size()); // Pre-allocate for efficiency
    for (char c : word) {
        letters.push_back({c, false, false, false});
    }
    return letters;
}

// Compare original word with guess and update guess letter status
bool compareWords(const vector<char>& original, vector<Letter>& guess, int length) {
    // First pass: mark correct positions
    for (int i = 0; i < length; i++) {
        if (original[i] == guess[i].letter) {
            guess[i].correct = true;
        }
    }
    
    // Second pass: mark wrong positions (only for non-correct letters)
    for (int i = 0; i < length; i++) {
        if (!guess[i].correct) {
            for (int j = 0; j < length; j++) {
                if (original[j] == guess[i].letter && !guess[j].correct) {
                    guess[i].wrongPosition = true;
                    break;
                }
            }
        }
    }
    
    // Check if all letters are correct (win condition)
    for (int i = 0; i < length; i++) {
        if (!guess[i].correct) {
            return false;
        }
    }
    return true;
}

// Create a vector of vectors of Letter structs
vector<vector<Letter>> createGuessesVector(int length) {
    vector<vector<Letter>> vec;
    vec.reserve(6); // Pre-allocate for typical game length
    return vec;
}

void showCustomWordMenu() {
    cout << "\n=== Custom Word Management ===" << endl;
    cout << "1. Add single custom word" << endl;
    cout << "2. Add multiple custom words (comma-separated)" << endl;
    cout << "3. Add custom words to databases (comma-separated)" << endl;
    cout << "4. View custom words" << endl;
    cout << "5. Remove custom word" << endl;
    cout << "6. Clear all custom words" << endl;
    cout << "7. Back to main menu" << endl;
    cout << "Choose an option: ";
}

void handleCustomWordManagement(SQLite::Database* db) {
    int choice;
    showCustomWordMenu();
    cin >> choice;
    
    switch (choice) {
        case 1: {
            string word;
            cout << "Enter custom word: ";
            cin >> word;
            if (addCustomWord(*db, word)) {
                cout << "Custom word added successfully!" << endl;
            } else {
                cout << "Failed to add custom word." << endl;
            }
            break;
        }
        case 2: {
            string input;
            cout << "Enter custom words (comma-separated): ";
            cin.ignore(); // Clear buffer
            getline(cin, input);
            vector<string> words = parseCommaSeparatedWords(input);
            if (addCustomWords(*db, words)) {
                cout << "All custom words added successfully!" << endl;
            } else {
                cout << "Some words failed to add." << endl;
            }
            break;
        }
        case 3: {
            string input;
            cout << "Enter custom words (comma-separated): ";
            cin.ignore(); // Clear buffer
            getline(cin, input);
            vector<string> words = parseCommaSeparatedWords(input);
            
            if (createCustomDatabase(words)) {
                cout << "Custom words added to databases successfully!" << endl;
            } else {
                cout << "Failed to add some custom words to databases." << endl;
            }
            break;
        }
        case 4: {
            vector<string> customWords = getCustomWords(*db);
            if (customWords.empty()) {
                cout << "No custom words found." << endl;
            } else {
                cout << "Custom words:" << endl;
                for (const string& word : customWords) {
                    cout << "- " << word << endl;
                }
            }
            break;
        }
        case 5: {
            string word;
            cout << "Enter word to remove: ";
            cin >> word;
            if (removeCustomWord(*db, word)) {
                cout << "Custom word removed successfully!" << endl;
            } else {
                cout << "Failed to remove custom word." << endl;
            }
            break;
        }
        case 6: {
            cout << "Are you sure you want to clear all custom words? (y/n): ";
            char confirm;
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                if (clearCustomWords(*db)) {
                    cout << "All custom words cleared successfully!" << endl;
                } else {
                    cout << "Failed to clear custom words." << endl;
                }
            }
            break;
        }
        case 7:
            return;
        default:
            cout << "Invalid option." << endl;
            break;
    }
}