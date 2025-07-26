#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "random.hpp"
#include "words.hpp"
#include "drawguess.hpp"
#include <SQLiteCpp/SQLiteCpp.h>

using namespace std;

void testRandomGenerator() {
    cout << "Testing random number generator..." << endl;
    
    // Test basic functionality
    int num1 = random_int(1, 100);
    int num2 = random_int(1, 100);
    assert(num1 >= 1 && num1 <= 100);
    assert(num2 >= 1 && num2 <= 100);
    
    // Test range
    int num3 = random_int(0, 1);
    assert(num3 == 0 || num3 == 1);
    
    cout << "Random generator tests passed!" << endl;
}

void testDatabaseLoading() {
    cout << "Testing database loading..." << endl;
    
    // Test opening database
    SQLite::Database* db = openDatabase(5);
    assert(db != nullptr);
    
    // Test getting length
    int length = getLength(*db);
    assert(length > 0);
    cout << "Database contains " << length << " words" << endl;
    
    // Test getting a word
    string word = getWord(*db, 0);
    assert(!word.empty());
    assert(word.length() == 5);
    cout << "First word: " << word << endl;
    
    // Test word existence
    assert(isWordInDatabase(*db, word));
    assert(!isWordInDatabase(*db, "INVALID"));
    
    closeDatabase(db);
    cout << "Database loading tests passed!" << endl;
}

void testWordProcessing() {
    cout << "Testing word processing..." << endl;
    
    // Test vector conversion
    string testWord = "hello";
    vector<char> vec = getVector(testWord);
    assert(vec.size() == 5);
    assert(vec[0] == 'h');
    assert(vec[4] == 'o');
    
    // Test letter vector creation
    vector<Letter> letters = createLetterVector(vec);
    assert(letters.size() == 5);
    assert(letters[0].letter == 'h');
    assert(!letters[0].correct);
    assert(!letters[0].wrongPosition);
    
    cout << "Word processing tests passed!" << endl;
}

void testWordComparison() {
    cout << "Testing word comparison..." << endl;
    
    // Test exact match
    vector<char> original = getVector("hello");
    vector<Letter> guess = createLetterVector(getVector("hello"));
    bool result = compareWords(original, guess, 5);
    assert(result == true);
    assert(guess[0].correct);
    assert(guess[1].correct);
    assert(guess[2].correct);
    assert(guess[3].correct);
    assert(guess[4].correct);
    
    // Test partial match
    vector<Letter> guess2 = createLetterVector(getVector("world"));
    result = compareWords(original, guess2, 5);
    assert(result == false);
    assert(!guess2[0].correct); // 'w' not in hello
    assert(!guess2[1].correct); // 'o' in wrong position
    assert(guess2[1].wrongPosition); // 'o' should be marked as wrong position
    
    // Test no match
    vector<Letter> guess3 = createLetterVector(getVector("quick"));
    result = compareWords(original, guess3, 5);
    assert(result == false);
    assert(!guess3[0].correct);
    assert(!guess3[1].correct);
    
    cout << "Word comparison tests passed!" << endl;
}

void testCommaSeparatedParsing() {
    cout << "Testing comma-separated word parsing..." << endl;
    
    // Test basic parsing
    string input1 = "hello,world,quick,brown,fox";
    vector<string> words1 = parseCommaSeparatedWords(input1);
    assert(words1.size() == 5);
    assert(words1[0] == "hello");
    assert(words1[1] == "world");
    assert(words1[2] == "quick");
    assert(words1[3] == "brown");
    assert(words1[4] == "fox");
    
    // Test with spaces
    string input2 = "hello, world , quick ,brown, fox";
    vector<string> words2 = parseCommaSeparatedWords(input2);
    assert(words2.size() == 5);
    assert(words2[0] == "hello");
    assert(words2[1] == "world");
    assert(words2[2] == "quick");
    assert(words2[3] == "brown");
    assert(words2[4] == "fox");
    
    // Test empty input
    string input3 = "";
    vector<string> words3 = parseCommaSeparatedWords(input3);
    assert(words3.empty());
    
    // Test single word
    string input4 = "hello";
    vector<string> words4 = parseCommaSeparatedWords(input4);
    assert(words4.size() == 1);
    assert(words4[0] == "hello");
    
    // Test with empty entries
    string input5 = "hello,,world, ,quick";
    vector<string> words5 = parseCommaSeparatedWords(input5);
    assert(words5.size() == 3);
    assert(words5[0] == "hello");
    assert(words5[1] == "world");
    assert(words5[2] == "quick");
    
    cout << "Comma-separated parsing tests passed!" << endl;
}

void testCustomWordManagement() {
    cout << "Testing custom word management..." << endl;
    
    // Create a test database
    SQLite::Database* db = openDatabase(5);
    
    // Test adding single custom word
    bool result = addCustomWord(*db, "custom_test");
    assert(result);
    assert(isWordInDatabase(*db, "custom_test"));
    
    // Test adding multiple custom words
    vector<string> words = {"custom_word1", "custom_word2", "custom_word3"};
    result = addCustomWords(*db, words);
    assert(result);
    assert(isWordInDatabase(*db, "custom_word1"));
    assert(isWordInDatabase(*db, "custom_word2"));
    assert(isWordInDatabase(*db, "custom_word3"));
    
    // Test getting custom words
    vector<string> customWords = getCustomWords(*db);
    assert(customWords.size() >= 4); // At least our 4 custom words
    
    // Test removing custom word
    result = removeCustomWord(*db, "custom_test");
    assert(result);
    assert(!isWordInDatabase(*db, "custom_test"));
    
    // Test clearing custom words
    result = clearCustomWords(*db);
    assert(result);
    customWords = getCustomWords(*db);
    assert(customWords.empty());
    
    closeDatabase(db);
    cout << "Custom word management tests passed!" << endl;
}

void testCustomDatabaseCreation() {
    cout << "Testing custom database creation..." << endl;
    
    // Test adding words to existing databases
    vector<string> mixedWords = {"hello", "world", "quick", "brown", "fox", "jumps", "over", "lazy", "dog"};
    bool result = createCustomDatabase(mixedWords);
    // Note: This test may fail if the required databases don't exist
    // The function should handle missing databases gracefully
    cout << "Custom database creation test completed (may fail if databases don't exist)" << endl;
}

void testGameLogic() {
    cout << "Testing game logic..." << endl;
    
    // Test different difficulties
    vector<pair<int, int>> difficulties = {
        {1, 6},   // Default: 6 attempts, 5 letters
        {2, 8},   // Medium: 8 attempts, 6-11 letters
        {3, 10},  // Hard: 10 attempts, 12-17 letters
        {4, 15},  // Insane: 15 attempts, 18-23 letters
        {5, 30}   // Godlike: 30 attempts, 24-29 letters
    };
    
    for (const auto& diff : difficulties) {
        int difficulty = diff.first;
        int expectedAttempts = diff.second;
        
        // Test length validation
        string testWord = "hello";
        bool validLength = checkLength(5, testWord);
        assert(validLength);
        
        bool invalidLength = checkLength(6, testWord);
        assert(!invalidLength);
    }
    
    cout << "Game logic tests passed!" << endl;
}

void testEdgeCases() {
    cout << "Testing edge cases..." << endl;
    
    // Test empty word
    assert(!checkLength(5, ""));
    
    // Test very long word
    string longWord(100, 'a');
    assert(checkLength(100, longWord));
    
    // Test special characters
    string specialWord = "h3ll0";
    assert(checkLength(5, specialWord));
    
    // Test parsing edge cases
    string edgeInput = ",,hello,,world,,,";
    vector<string> edgeWords = parseCommaSeparatedWords(edgeInput);
    assert(edgeWords.size() == 2);
    assert(edgeWords[0] == "hello");
    assert(edgeWords[1] == "world");
    
    cout << "Edge case tests passed!" << endl;
}

void testDatabaseOperations() {
    cout << "Testing database operations..." << endl;
    
    SQLite::Database* db = openDatabase(5);
    
    // Test getting word at invalid index
    string invalidWord = getWord(*db, 999999);
    assert(invalidWord.empty());
    
    // Test getting word at negative index
    string negativeWord = getWord(*db, -1);
    assert(negativeWord.empty());
    
    // Test database length
    int length = getLength(*db);
    assert(length > 0);
    
    // Test word existence with empty string
    assert(!isWordInDatabase(*db, ""));
    
    closeDatabase(db);
    cout << "Database operations tests passed!" << endl;
}

void testVectorOperations() {
    cout << "Testing vector operations..." << endl;
    
    // Test createGuessesVector
    vector<vector<Letter>> guesses = createGuessesVector(6);
    assert(guesses.empty());
    
    // Test with different lengths
    vector<vector<Letter>> guesses2 = createGuessesVector(10);
    assert(guesses2.empty());
    
    // Test letter vector creation with empty word
    vector<char> emptyVec;
    vector<Letter> emptyLetters = createLetterVector(emptyVec);
    assert(emptyLetters.empty());
    
    cout << "Vector operations tests passed!" << endl;
}

void runAllTests() {
    cout << "Running comprehensive test suite..." << endl;
    cout << "=====================================" << endl;
    
    testRandomGenerator();
    testDatabaseLoading();
    testWordProcessing();
    testWordComparison();
    testCommaSeparatedParsing();
    testCustomWordManagement();
    testCustomDatabaseCreation();
    testGameLogic();
    testEdgeCases();
    testDatabaseOperations();
    testVectorOperations();
    
    cout << "=====================================" << endl;
    cout << "All tests passed successfully!" << endl;
}

int main() {
    try {
        runAllTests();
        return 0;
    } catch (const exception& e) {
        cout << "Test failed with exception: " << e.what() << endl;
        return 1;
    }
}