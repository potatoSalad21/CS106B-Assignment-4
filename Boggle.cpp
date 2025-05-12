/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: extend the documentation]
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>
#include "gboggle.h"
#include "StanfordCPPLib/gwindow.h"
#include "StanfordCPPLib/lexicon.h"
#include "StanfordCPPLib/random.h"
#include "StanfordCPPLib/simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;

const string STANDARD_CUBES[16]  = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25]  = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

const int HIGHLIGHT_TIME = 200;

/* Function prototypes */

void welcome();
void giveInstructions();
void runGame();
vector<string> createBoard();
vector<string> createRandomBoard();
string getUserInput();
void randomizeCubes(vector<string>&);
void drawText(vector<string>&);
void playerTurn(int&, vector<string>&);
void computerTurn(int&, vector<string>&);
void checkGameState(int, int);
void calcScore(int&, unordered_set<string>&);
void backtrack(vector<string>&, int, int, string&, vector<vector<bool>>&, unordered_set<string>&);
bool dfs(vector<string>&, int, int, string&, vector<pair<int, int>>&, int);
bool checkWord(vector<string>&, string&, vector<pair<int, int>>&);
void showTrail(vector<pair<int, int>>&);

/* Global variables */

Lexicon engWords("EnglishWords.dat");
vector<vector<int>> dirForDfs = {
    {1,0}, {0,1}, {-1,0}, {0,-1}
};

/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
    welcome();
    giveInstructions();
    initGBoggle(gw);
    runGame();

    return 0;
}

// main game loop
void runGame() {
    while (true) {
        vector<string> board = createBoard();
        drawBoard(4, 4);
        drawText(board);

        int playerRes = 0;
        int computerRes = 0;
        playerTurn(playerRes, board);
        computerTurn(computerRes, board);
        checkGameState(playerRes, computerRes);

        string resp = getLine("Stop playing? (type 'exit'): ");
        if (resp == "exit") break;
    }
}

vector<string> createBoard() {
    string input = getUserInput();
    vector<string> board;
    if (input.empty())
        board = createRandomBoard();
    else {
        string curr = "";
        for (int i = 0; i < input.size(); i++) {
            curr += toupper(input[i]);
            if (curr.size() == 4) {
                board.push_back(curr);
                curr = "";
            }
        }
    }

    return board;
}

void drawText(vector<string> &board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[0].size(); j++) {
            labelCube(i, j, board[i][j]);
        }
    }
}

void playerTurn(int &score, vector<string> &board) {
    unordered_set<string> guesses;
    while (true) {
        string input = getLine("Enter a word (must be over 4 letters / [Return] key finishes the turn): ");
        transform(input.begin(), input.end(), input.begin(), ::toupper);
        if (input.empty()) return;
        else if (input.size() < 4) continue;
        else if (!engWords.contains(input))
            cout << "Invalid input, couldn't find the word" << endl;
        else if (guesses.count(input))
            cout << "This world is already uesed" << endl;
        else {
            vector<pair<int, int>> choiceTrail;
            if (!checkWord(board, input, choiceTrail)) {
                cout << "Couldn't form the word" << endl;
                continue;
            }
            recordWordForPlayer(input, Player::HUMAN);
            score += input.size() - 3;
            guesses.insert(input);
            showTrail(choiceTrail);
        }
    }
}

// check if the word trail can be built
bool checkWord(vector<string>& board, string& input, vector<pair<int, int>>& choiceTrail) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[0].size(); j++) {
            if (board[i][j] != input[0])
                continue;

            vector<pair<int, int>> copy;
            if (dfs(board, i, j, input, copy, 0)) {
                choiceTrail = copy;
                return true;
            }
        }
    }

    return false;
}

// standard dfs not much to add
bool dfs(vector<string> &board, int i, int j, string &input, vector<pair<int, int>> &choiceTrail, int start) {
    // base-cases
    if (start == input.size())
        return true;
    if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size())
        return false;
    if (board[i][j] == '#' || board[i][j] != input[start])
        return false;

    char curr = board[i][j];
    board[i][j] = '#';
    choiceTrail.push_back(make_pair(i, j));
    for (vector<int> dir : dirForDfs) {
        int r = i + dir[0];
        int c = j + dir[1];
        if (dfs(board, r, c, input, choiceTrail, start+1)) {
            board[i][j] = curr;
            return true;
        }
    }

    board[i][j] = curr;
    choiceTrail.pop_back();
    return false;
}

void computerTurn(int &score, vector<string> &board) {
    unordered_set<string> wordSet;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[0].size(); j++) {
            vector<vector<bool>> visited(board.size(), vector<bool>(board[0].size(), false));
            string res = "";
            backtrack(board, i, j, res, visited, wordSet);
        }
    }
    calcScore(score, wordSet);
}

void backtrack(vector<string> &board, int i, int j, string &res,
         vector<vector<bool>> &visited, unordered_set<string> &wordSet) {

    if (visited[i][j]) return;

    res += board[i][j];
    if (engWords.contains(res) && res.size() >= 4) {
        wordSet.insert(res);
    }
    if (!engWords.containsPrefix(res)) {
        res.pop_back();
        return;
    }

    visited[i][j] = true;
    for (vector<int> dir : dirForDfs) {
        int r = i + dir[0];
        int c = j + dir[1];
        backtrack(board, r, c, res, visited, wordSet);
    }
    visited[i][j] = false;
    res.pop_back();
}

void calcScore(int &score, unordered_set<string> &wordSet) {
    for (auto &word : wordSet) {
        score += word.size()-3;
        recordWordForPlayer(word, Player::COMPUTER);
    }
}

void checkGameState(int usrScore, int compScr) {
    cout << (usrScore > compScr ? "YOU WON!!!" : "Computer won ") << endl;
}

void showTrail(vector<pair<int, int>>& trail) {
    for (auto& [r, c] : trail) {
        highlightCube(r, c, true);
        pause(HIGHLIGHT_TIME);
        highlightCube(r, c, false);
    }
}

// shuffles the board according to the standard cubes
vector<string> createRandomBoard() {
    vector<string> board;

    vector<string> cubes;
    for (int i = 0; i < 16; i++) {
        cubes.push_back(STANDARD_CUBES[i]);
    }
    randomizeCubes(cubes);

    string curr = "";
    for (int i = 0; i < cubes.size(); i++) {
        curr += cubes[i][randomInteger(0, cubes[i].size()-1)];
        if (curr.size() == 4) {
            board.push_back(curr);
            curr = "";
        }
    }

    return board;
}

// gets valid user input for the game mode and game letters
string getUserInput() {
    string gameMode = getLine("Choose a game mode (random / custom): ");
    if (gameMode == "random")
        return "";

    string letters;
    while (true) {
        letters = getLine("Enter 16 letters for the game: ");
        if (letters.size() == 16) break;
    }

    return letters;
}

void randomizeCubes(vector<string> &cubes) {
    for (int i = 0; i < cubes.size(); i++) {
        int randIdx = randomInteger(i, cubes.size()-1);
        swap(cubes[i], cubes[randIdx]);
    }
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}

