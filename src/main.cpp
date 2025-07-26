#include <iostream>
#include <vector>
#include "random.hpp"
#include "words.hpp"
#include "drawguess.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

using namespace std;

int main() {
    bool play = true;
    while (play) {
        int difficulty = 1;
        int attempts = 5;
        int length = 5; // Default to 5-letter words for custom word management
        SQLite::Database* db = {};
        vector<Letter> guess = {};
        int player_attempts = {};
        vector<vector<Letter>> guesses = createGuessesVector(6);
        
        cout << "Choose a difficulty: " << endl;
        cout << "1. Default" << endl;
        cout << "2. Medium" << endl;
        cout << "3. Hard" << endl;
        cout << "4. Insane" << endl;
        cout << "5. Godlike" << endl;
        cout << "6. Custom Word Management" << endl;
        cout << "7. Exit" << endl;
        cin >> difficulty;

        switch (difficulty) {
            case 1:
                attempts = 6;
                length = 5;
                db = openDatabase(length);
                cout << "The word length is: " << length << endl;
                cout << "The number of attempts is: " << attempts << endl;
                break;
            case 2:
                attempts = 8;
                length = random_int(6, 11);
                db = openDatabase(length);
                cout << "The word length is: " << length << endl;
                cout << "The number of attempts is: " << attempts << endl;
                break;  
            case 3:
                attempts = 10;
                length = random_int(12, 17);
                db = openDatabase(length);
                cout << "The word length is: " << length << endl;
                cout << "The number of attempts is: " << attempts << endl;
                break;
            case 4:
                attempts = 15;
                length = random_int(18, 23);
                db = openDatabase(length);
                cout << "The word length is: " << length << endl;
                cout << "The number of attempts is: " << attempts << endl;
                break;
            case 5:
                attempts = 30;
                length = random_int(24, 29);
                db = openDatabase(length);
                cout << "The word length is: " << length << endl;
                cout << "The number of attempts is: " << attempts << endl;
                break;
            case 6: {
                // Use default 5-letter database for custom word management in write mode
                db = openDatabaseForWriting(length);
                handleCustomWordManagement(db);
                closeDatabase(db);
                continue; // Skip to next iteration without starting a game
            }
            case 7:
                play = false;
                continue;
            default:
                cout << "Invalid difficulty, defaulting to standard" << endl;
                attempts = 6;
                difficulty = 1;
                length = 5;
                db = openDatabase(length);
                cout << "The word length is: " << length << endl;
                cout << "The number of attempts is: " << attempts << endl;
                break;
        }

        string word = getWord(*db, random_int(0, getLength(*db) - 1));
        vector<char> word_vector = getVector(word);
        bool win = false;
        cout << "Enter a word: " << endl;
        while (player_attempts < attempts) {
            string player_guess;
            cin >> player_guess;
            clearScreen();
            if (checkLength(length, player_guess)) {
                if (isWordInDatabase(*db, player_guess)) {
                    guess = createLetterVector(getVector(player_guess));
                    if (compareWords(word_vector, guess, length)) {
                        cout << "You win!" << endl;
                        win = true;
                        break;
                    } else {
                        cout << "The number of attempts is: " << attempts - player_attempts << endl;
                        guesses.push_back(guess);
                        printAll(guesses);
                        player_attempts++;
                    }
                } else {
                    cout << "Word is not in the database. Try again." << endl;
                }
            } else {
                cout << "Invalid word length" << endl;
            }
        }
        if (win) {
            cout << "You win!" << endl;
            cout << "Want to play again? (y/n)" << endl;
            char play_again;
            cin >> play_again;
            if (play_again == 'y') {
                play = true;
            } else {
                play = false;
            }
        } else {
            cout << "You lose!" << endl;
            cout << "The word length is: " << length << endl;
            cout << "The word was: " << word << endl;
            cout << "Want to play again? (y/n)" << endl;
            char play_again;
            cin >> play_again;
            if (play_again == 'y') {
                play = true;
            } else {
                play = false;
            }
        }
        closeDatabase(db);
    }
    return 0;
}
