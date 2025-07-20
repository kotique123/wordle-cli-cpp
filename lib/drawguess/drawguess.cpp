#include <vector>
#include <iostream>
#include "drawguess.hpp"

using namespace std;

void printGuess(vector<Letter>& guess) {
    for (int i = 0; i < guess.size(); i++) {
        if (guess[i].correct) {
            cout << GREEN << guess[i].letter << RESET;
        }
        else if (guess[i].wrongPosition) {
            cout << YELLOW << guess[i].letter << RESET;
        } else {
            cout << guess[i].letter;
        }
    }
    cout << endl;
}

void addGuess(const vector<Letter>& guess, vector<vector<Letter>>& guesses) {
    guesses.push_back(guess);
}

void printAll(const vector<vector<Letter>>& guesses) {
    for (vector<Letter> guess : guesses) {
        printGuess(guess);
    }
}

void clearScreen() {
    cout << "\x1B[2J\x1B[H";
    cout.flush();
}
