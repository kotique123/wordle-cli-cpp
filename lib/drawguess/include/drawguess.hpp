#include <vector>
#include "words.hpp"

using namespace std;

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void printGuess(vector<Letter>& guess);

void addGuess(const vector<Letter>& guess, vector<vector<Letter>>& guesses);

void printAll(const vector<vector<Letter>>& gueses);

void clearScreen();