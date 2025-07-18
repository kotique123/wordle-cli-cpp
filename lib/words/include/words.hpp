#pragma once
#include <string>
#include <vector>
#include "SQLiteCpp/SQLiteCpp.h"

using namespace std;

struct Letter {
    char letter;
    bool wasPicked;
    bool wrongPosition;
    bool correct;
};

// Check if word length matches expected length
bool checkLength(int length, const string& word);

// Get the number of rows in the words table
int getLength(SQLite::Database& db);

// Get a word at the specified index (0-based)
string getWord(SQLite::Database& db, int index);

// Convert a word string into a vector of characters
vector<char> getVector(const string& word);

// Check if a word exists in the database
bool isWordInDatabase(SQLite::Database& db, const string& word);

// Create a vector of Letter structs from a word
vector<Letter> createLetterVector(const vector<char>& word);

// Compare original word with guess and update guess letter status
bool compareWords(const vector<char>& original, vector<Letter>& guess, int length);