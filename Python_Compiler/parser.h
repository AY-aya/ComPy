#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include "AST_Node.h"
#include "ParserSymbolTable.h"

struct ParseError {
    int line;
    int col;
    std::string message;
    ParseError(int l, int c, const std::string& msg)
        : line(l), col(c), message(msg) {}
};


class Parser : public QObject
{
    Q_OBJECT
public:

private:
    vector<Token> tokens;
    int currentTokenIndex = 0;
    Token currentToken;
    vector<ParseError> errors;
    vector<int> indentStack;
    ParserSymbolTable& symbolTable;  // Reference to symbol table from lexer

    string getValueFromNode(const unique_ptr<ASTNode>& node);

    string getTypeFromNode(const unique_ptr<ASTNode>& node);

    string getValueFromNode(ASTNode *node);
    string evaluateExpression(ASTNode *node);
    bool isValidStatementStart(const string& id);
public:
    // Take symbol table as reference in constructor
    explicit Parser(vector<Token> tokens, ParserSymbolTable& symTab, QObject *parent = nullptr);

    ParserSymbolTable getSymbolTable(){
        return symbolTable;
    }

    vector<ParseError> getErrors(){
        return errors;
    }


    Token advance();

    void expectIndent(int expectedLevel);

    Token peek();

    Token peekNextToken();

    bool match(TokenType type, const string& value = "");

    void expect(TokenType type, const string& errorMsg, const string& value = "");

    void synchronize();

    unique_ptr<ProgramNode> parseProgram();


    unique_ptr<ASTNode> parseStmt();

    unique_ptr<ASTNode> parseFuncCall();

    unique_ptr<ASTNode> parseAssignStmt();

    unique_ptr<ASTNode> parseReturnStmt();

    unique_ptr<ASTNode> parseIfStmt();

    unique_ptr<ASTNode> parseElseStmt();

    unique_ptr<ASTNode> parseForStmt();

    unique_ptr<ASTNode> parseWhileStmt();

    unique_ptr<ASTNode> parseFuncDef();

    unique_ptr<ASTNode> parseFuncCallStmt();

    unique_ptr<ASTNode> parseExpr();

    unique_ptr<ASTNode> parseLogicalOr();

    unique_ptr<ASTNode> parseLogicalAnd();

    unique_ptr<ASTNode> parseEquality();

    unique_ptr<ASTNode> parseRelational();

    unique_ptr<ASTNode> parseTerm();

    unique_ptr<ASTNode> parseFactor();

    unique_ptr<ASTNode> parseUnary();

    unique_ptr<ASTNode> parsePrimary();

    void addError(int line, int col, const std::string& msg);

    void printErrors();

signals:
};

#endif // PARSER_H
