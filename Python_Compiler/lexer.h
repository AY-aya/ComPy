#ifndef LEXER_H
#define LEXER_H

#include <QObject>
#include "AST_Node.h"
#include <SymbolTable.h>

using namespace std;

class Lexer : public QObject
{
    Q_OBJECT
public:
    explicit Lexer(QObject *parent = nullptr);

public:
    // Constructor
    Lexer(const std::string& input) : input(input) {}

    // Tokenizes the entire input and returns all tokens
    vector<Token> tokenize();

    // Prints tokens in the required format
    void printTokens(const vector<Token>& tokens);


    // Prints the symbol table contents
    void printSymbolTable();

    void printErrors(const vector<Token>& tokens);


    std::vector<SymbolTableEntry> getSymbolTable();

private:
    string input;
    size_t pos = 0; // points to the current character you're looking at in the input string.
    int line = 1, column = 1;
    SymbolTable symbolTable;
    bool newLine;
    // List of all Python keywords
    vector<string> keywords = {
        "False", "None", "True", "and", "as", "assert",
        "async", "await", "break", "class", "continue",
        "def", "del", "elif", "else", "except", "finally",
        "for", "from", "global", "if", "import", "in",
        "is", "lambda", "nonlocal", "not", "or", "pass",
        "raise", "return", "try", "while", "with", "yield"
    };

    // Returns the current character without advancing
    char peek(int offset = 0) const;

    // Every time you advance(), you move pos forward by one character
    // Advances to the next character and updates line/column
    char advance();

    // Checks if a string is a keyword
    bool isKeyword(const string& word);

    // Skips whitespace (excluding newlines)
    void skipWhitespace();

    // Skips over Python-style comments
    void skipComment();

    // Extracts and returns an identifier or keyword token
    Token getIdentifier();

    // Extracts and returns a numeric token
    Token getNumber();

    // Extracts and returns a string token
    Token getString();

    // Extracts and returns an operator token
    Token getOperator();

    //for handling indentations
    int getIndentLevel();

    // Detects types and updates symbol table entries accordingly
    void detectTypes();


signals:
};

#endif // LEXER_H
