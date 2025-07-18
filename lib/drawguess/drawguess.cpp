#include "drawguess.hpp"

using namespace std;

void printResult(vector<Letter>& guess) {
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