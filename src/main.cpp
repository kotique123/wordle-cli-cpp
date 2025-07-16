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
 * TODO: #1 Random number generator
 *         * Seed = unix time + some value from device to make it random
 *         * Random number is being generated within limits of words<n>.txt file.
 * 
 * TODO: #2 File handler
 *         * Be able to read file get number of lines in file.
 *         * Be able to read specific line from a file.
 * TODO:  
 *
 */
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

const string RED {"\e[31m"}; 

int main()
{
    cout << RED << "Hello, World!" << endl;
    return 0;
}
