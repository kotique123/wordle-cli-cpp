/* Wordle-cli game structure
 * 1. Ask for difficulty where default means 5 letter word with 6 attempts.
 * 2. Generate a random seed Time + some sort of device id.
 * 3. Ask user for a guess and check if guess is valid.
 *      * compare guess to a word.
 *      * check if guess is even a word based on a list of words.
 *      * if guess is a word, and is not a target word:
 *          * check what letters match exactly.
 *          * check what letters are there but are in different places.
 *          * return the result
 * 4. Repeat step 3 (game loop)
 * 5. If guess is sucessfull print win message and ask if a user wants to play again
 *      * if a user does not want to play the game, exit
 *      * if user want to play the game again, return back to step 1.
 * Data structures:
 *  enum for difficulties
 *  an arraylist of fixed size in which a word is disected so that we can determine which letters are there
 *  array of arrys that would allow us to print our guesses
 *  Maybe:
 *  a struct that would represent a guessed word/letter with color         
 *
 *
 *
 * TODO: #1 Random number generator - Done
 *         * Seed = unix time + some value from device to make it random
 *         * Random number is being generated within limits of words<n>.txt file.
 * 
 * TODO: #2 File handler - Redisigned. Move with SQLite
 *         * Be able to read file get number of lines in file.
 *         * Be able to read specific line from a file.
 *
 * TODO: #3 Database handler
 *         * Access a desired word database.
 * TODO: #4 Drawing game in a consistent way
 *         1. Accept input
 *         2. Process it
 *         3. Put proccessed input in an array or vector
 *         4. Print it.
 *         Clear the screen after accepting input
 *
 *
 */
 
#include <iostream>
#include <vector>
#include "random.hpp"
#include "words.hpp"
#include "drawguess.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
#include <SQLiteCpp/VariadicBind.h>

using namespace std;

const string RED {"\e[31m"}; 

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
            //cout << "Trying to open database at: " << path << endl;
            return new SQLite::Database(path);
        } catch (const SQLite::Exception& e) {
            cout << "Failed to open " << path << ": " << e.what() << endl;
        }
    }
    
    throw runtime_error("Could not open database file from any expected location");
}

int main()
{
    try {
        SQLite::Database* db = openDatabase();
        
        int size = getLength(*db);
        string word = getWord(*db, random_int(0, size - 1));
        vector<char> original = getVector(word);
        //cout << "Word: " << word << endl;
        bool guessed = false;
        int attempts = 0;
        while (!guessed && attempts < 6) {
            string guess;
            //cout << "Enter a guess: ";
            cin >> guess;
            cout << "\x1B[2J\x1B[H";
            cout.flush();
            if (checkLength(5, guess)) {
                if (isWordInDatabase(*db, guess)) {
                    vector<Letter> guessVector = createLetterVector(getVector(guess));
                    if (compareWords(original, guessVector, 5)) {
                        //printResult(guessVector);
                        cout << "you win" << endl;
                        guessed = true;
                        attempts++;
                    }
                    else {
                        //printResult(guessVector);
                        attempts++;
                        //cout << "Try again" << endl;
                    }
                } else {
                    cout << "Word not in database!" << endl;
                }
            } else {
                cout << "Word must be 5 letters long!" << endl;
            }
        }
        if (attempts == 6) {
            cout << "You lose! The word was: " << word << endl;
        }
        delete db;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
