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
#include "words.hpp"

using namespace std;

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








