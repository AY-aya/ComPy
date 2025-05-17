#include "lexer.h"
#include <regex>

Lexer::Lexer(QObject *parent)
    : QObject{parent}
{}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    newLine = true;  // Track start of a new line

    while (true) {

        //for handling indentations
        if (newLine) {
            int currentIndent = getIndentLevel();
            while (peek() == ' ' || peek() == '\t') advance();

            // Emit one Indent token per new line, with the indent level in "tabs" (assuming 4 spaces/tab)
            if (currentIndent >= 0) {
                int indentLevel = currentIndent / 4;
                tokens.push_back({ TokenType::Indent, to_string(indentLevel), line, column });
            }

            newLine = false;
        }

        skipWhitespace();
        skipComment();

        char c = peek();
        int startCol = column;

        if (c == '\0') {
            tokens.push_back({ TokenType::EOFToken, "EOF", line, column });
            break;
        } else if (isalpha(c) || c == '_') {
            tokens.push_back(getIdentifier());
        } else if (isdigit(c)) {
            tokens.push_back(getNumber());
        } else if (c == '"') {
            tokens.push_back(getString());
        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ||
                   c == '<' || c == '>' || c == '!' || c == '.') {
            tokens.push_back(getOperator());
        } else if (c == '(' || c == ')' || c == ':' || c == ',') {
            tokens.push_back({ TokenType::Delimiter, std::string(1, advance()), line, startCol });
        } else if (c == '\n') {
            advance(); // skip newline
            newLine = true;
        } else {
            // Unrecognized character
            tokens.push_back({ TokenType::Error, std::string(1, advance()), line, startCol });
        }
    }

    detectTypes();
    return tokens;
}

void Lexer::printTokens(const vector<Token> &tokens) {
    cout << "Tokens:\n";
    int count = 1;
    for (const auto& token : tokens) {
        if (token.type == TokenType::EOFToken) break;
        if (token.type == TokenType::Error) continue;  //Skip error tokens
        if (token.type == TokenType::Indent) continue;  //Skip indent tokens

        cout << count++ << ". <";

        switch (token.type) {
        case TokenType::Identifier:
            cout << "identifier, " << token.symbolId;
            break;
        case TokenType::Keyword:
            cout << "keyword, " << token.value;
            break;
        case TokenType::Number:
            cout << "number, " << token.value;
            break;
        case TokenType::String:
            cout << "string, \"" << token.value << "\"";
            break;
        case TokenType::Operator:
            cout << "operator, " << token.value;
            break;
        case TokenType::Delimiter:
            cout << token.value;
            break;
        case TokenType::Indent:
            cout << "Indent, "<<token.value;
            break;
        default:
            cout << "unknown";
        }

        cout << "> at line " << token.line << ", column " << token.column << '\n';
    }
}

void Lexer::printSymbolTable() {
    symbolTable.print();
}

void Lexer::printErrors(const vector<Token> &tokens) {
    cout << "\nLexical Errors:\n";
    int count = 1;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Error) {
            cout << count++ << ". Invalid token: \"" << token.value
                 << "\" at line " << token.line << ", column " << token.column << '\n';
        }
    }
}

std::vector<SymbolTableEntry> Lexer::getSymbolTable()
{
    return symbolTable.getSymbolTable();
}

char Lexer::peek(int offset) const {
    if (pos + offset < input.size()) {
        return input[pos + offset];  // Look ahead by offset
    } else {
        return '\0';  // End of input
    }
}

char Lexer::advance() {
    char c = peek();
    pos++;
    column++;

    if (c == '\n') {
        line++;
        column = 1;
        newLine = true;
    } else if (newLine && c != ' ' && c != '\t') {
        newLine = false;
    }

    return c;
}

bool Lexer::isKeyword(const string &word) {
    return find(keywords.begin(), keywords.end(), word) != keywords.end();
}

void Lexer::skipWhitespace() {
    while (isspace(peek()) && peek() != '\n') advance();
}

void Lexer::skipComment() {
    if (peek() == '#') {
        while (peek() != '\n' && peek() != '\0') advance();
    }
}

Token Lexer::getIdentifier() {
    int startCol = column;
    string value;
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    if (isKeyword(value)) {
        return { TokenType::Keyword, value, line, startCol };
    } else {
        int id = symbolTable.insert(value);
        return { TokenType::Identifier, value, line, startCol, id };
    }
}

Token Lexer::getNumber() {
    int startCol = column;
    string value;

    // Handle leading zero formats
    if (peek() == '0') {
        value += advance();
        char next = tolower(peek());

        //handle hexa- octal- binary numbers
        if (next == 'x' || next == 'o' || next == 'b') {
            value += advance(); // consume x, o, or b
            int base = (next == 'x') ? 16 : (next == 'o') ? 8 : 2;

            bool hasValidDigit = false;

            while (isalnum(peek())) {
                char c = peek();

                // Validate digit based on base
                if ((base == 2 && (c != '0' && c != '1')) ||
                    (base == 8 && (c < '0' || c > '7')) ||
                    (base == 16 && !isxdigit(c))) {
                    return { TokenType::Error, "Invalid digit for base " + to_string(base) + ": " + value + c, line, startCol };
                }

                value += advance();
                hasValidDigit = true;
            }

            if (!hasValidDigit) {
                return { TokenType::Error, "Expected digits after prefix: " + value, line, startCol };
            }

            return { TokenType::Number, value, line, startCol };
        }

        // If not one of the special bases, check for leading zero error ex: 023
        if (isdigit(peek())) {
            while (isdigit(peek())) value += advance();
            return { TokenType::Error, "Invalid number with leading zero: " + value, line, startCol };
        }
    }

    // Decimal number
    while (isdigit(peek())) value += advance();

    // Handle float
    if (peek() == '.') {
        value += advance();
        while (isdigit(peek())) value += advance();
    }

    // Check for invalid trailing characters (e.g., 123abc)
    if (isalpha(peek()) || peek() == '_') {
        while (isalnum(peek()) || peek() == '_') {
            value += advance();
        }
        return { TokenType::Error, value, line, startCol };
    }

    return { TokenType::Number, value, line, startCol };
}

Token Lexer::getString() {
    int startCol = column;
    string value;

    // Check if it's """ or "
    advance(); // skip first "
    bool isTriple = false;

    if (peek() == '"') {
        advance(); // second "
        if (peek() == '"') {
            advance(); // third "
            isTriple = true;
        } else {
            // It was just "", treat as empty string
            return { TokenType::String, "", line, startCol };
        }
    }

    while (true) {
        if (peek() == '\0') {
            // EOF reached before closing quotes
            return { TokenType::Error, "Unterminated string", line, startCol };
        }

        if (!isTriple && peek() == '\n') {
            // Newline in a single-quote string is an error
            return { TokenType::Error, "Unterminated string", line, startCol };
        }

        if (isTriple) {
            if (peek() == '"' && peek(1) == '"' && peek(2) == '"') {
                advance(); advance(); advance(); // Skip closing """
                break;
            }
        } else {
            if (peek() == '"') {
                advance(); // Skip closing "
                break;
            }
        }

        value += advance();
    }

    return { TokenType::String, value, line, startCol };
}

Token Lexer::getOperator() {
    int startCol = column;
    string value;
    char c = advance();
    value += c;

    // Handle compound operators like ==, !=, <=, >=
    if ((c == '=' || c == '!' || c == '<' || c == '>') && peek() == '=') {
        value += advance();
    }

    // Handle '.' as a standalone operator (exclude float cases later in detectNumber)
    if (c == '.') {
        // You could add optional logic here to skip if it's part of a float,
        // but it's often better to handle that in number detection
        return { TokenType::Operator, ".", line, startCol };
    }

    return { TokenType::Operator, value, line, startCol };
}

int Lexer::getIndentLevel() {
    int level = 0;
    size_t tempPos = pos;

    while (input[tempPos] == ' ' || input[tempPos] == '\t') {
        if (input[tempPos] == ' ') level += 1;
        else if (input[tempPos] == '\t') level += 4; // 1 tab = 4 spaces (Python standard)
        tempPos++;
    }

    return level;
}

void Lexer::detectTypes() {
    regex assignRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*([^\n#]+))");
    regex funcRegex(R"(\bdef\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\()");
    smatch match;

    string::const_iterator searchStart(input.cbegin());

    // First: detect variable assignments
    while (regex_search(searchStart, input.cend(), match, assignRegex)) {
        string var = match[1];
        string expr = match[2];

        string type = "unknown";
        string cleanedExpr = regex_replace(expr, regex(R"(\s+)"), "");  // Remove spaces

        // Check if the expression is a literal value
        if (regex_match(cleanedExpr, regex(R"([0-9]+)"))) {  // Integer
            type = "int";
            symbolTable.updateType(var, type, cleanedExpr);
        }
        else if (regex_match(cleanedExpr, regex(R"([0-9]+\.[0-9]+)"))) {  // Float
            type = "float";
            symbolTable.updateType(var, type, cleanedExpr);
        }
        else if (regex_match(cleanedExpr, regex(R"(\".*\"|\'.*\')"))) {  // String
            type = "string";
            symbolTable.updateType(var, type, cleanedExpr);
        }
        else if (regex_match(cleanedExpr, regex(R"(True|False)"))) {  // Boolean
            type = "bool";
            symbolTable.updateType(var, type, cleanedExpr);
        }
        else {
            // The value is a complex expression or function call -> value remains unknown
            symbolTable.updateType(var, type);  // Just update the type to "unknown"
        }

        searchStart = match.suffix().first;
    }

    // Second: detect function declarations
    regex callRegex(R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*\()");
    searchStart = input.cbegin();
    while (regex_search(searchStart, input.cend(), match, callRegex)) {
        string funcName = match[1];
        symbolTable.updateType(funcName, "function");
        searchStart = match.suffix().first;
    }
}

