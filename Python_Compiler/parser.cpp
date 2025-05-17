#include "parser.h"

Parser::Parser(vector<Token> tokens, ParserSymbolTable &symTab, QObject *parent)
    : tokens(tokens), symbolTable(symTab) {
    indentStack.push_back(0);
    advance();
}

Token Parser::advance() {
    if (currentTokenIndex < tokens.size()) {
        currentToken = tokens[currentTokenIndex++];
    }
    return currentToken;
}

void Parser::expectIndent(int expectedLevel) {
    if (currentToken.type != TokenType::Indent) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Expected indentation at start of block");
        return;
    }

    int actualLevel = stoi(currentToken.value);
    if (actualLevel != expectedLevel) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Incorrect indentation level");
    }
    advance();
    if(currentToken.type == TokenType::Indent){
        expectIndent(actualLevel);
    }
}

Token Parser::peek() {
    return currentToken;
}

Token Parser::peekNextToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    return {TokenType::EOFToken, "EOF", 0, 0};
}

bool Parser::match(TokenType type, const string &value) {
    if (currentToken.type == type && (value.empty() || currentToken.value == value)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const string &errorMsg, const string &value) {
    if (!match(type, value)) {
        errors.emplace_back(currentToken.line, currentToken.column, errorMsg);
        synchronize();
    }
}

void Parser::synchronize() {
    vector<string> syncKeywords = {"def", "if", "while", "return"};
    while (currentToken.type != TokenType::EOFToken && currentToken.type != TokenType::Indent &&
           (currentToken.type != TokenType::Keyword ||
            find(syncKeywords.begin(), syncKeywords.end(), currentToken.value) == syncKeywords.end())) {
        advance();
    }
}


unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = make_unique<ProgramNode>();
    while (currentToken.type != TokenType::EOFToken) {
        if (currentToken.type == TokenType::Indent) {
            expectIndent(0);  // Expect no indentation at top level
        }
        auto stmt = parseStmt();
        if (stmt) {
            program->addChild(move(stmt));
        }
    }
    return program;
}


bool Parser::isValidStatementStart(const string& id) {
    // Allow only these identifier-starting statements:
    return peekNextToken().value == "=" ||  // Assignment
           peekNextToken().value == "(";    // Function call
}
unique_ptr<ASTNode> Parser::parseStmt() {
    if (currentToken.type == TokenType::EOFToken) {
        return nullptr;
    }

    if (currentToken.type == TokenType::Identifier) {
        if (!isValidStatementStart(currentToken.value)) {
            errors.emplace_back(currentToken.line, currentToken.column,
                                "Invalid statement starting with identifier: " + currentToken.value);
            advance();
            return nullptr;
        }
    }

    switch (currentToken.type) {
    case TokenType::Keyword: {
        if (currentToken.value == "if") return parseIfStmt();
        if (currentToken.value == "while") return parseWhileStmt();
        if (currentToken.value == "def") return parseFuncDef();
        if (currentToken.value == "return") return parseReturnStmt();
        if (currentToken.value == "for") return parseForStmt();
        if (currentToken.value == "elif" || currentToken.value == "else") {
            //check scope
            if(symbolTable.getCurrentScope() == "if block")
                return nullptr; //handled in ifstmt
            else {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Unexpected token at start of statement: " +currentToken.value);
                synchronize();
                return nullptr;
            }
        }
        break;
    }
    case TokenType::Identifier: {
        if (peekNextToken().value == "(") {
            return parseFuncCallStmt();
        } else {
            return parseAssignStmt();
        }
    }
    default:
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Unexpected token at start of statement: " +currentToken.value);
        advance();
        return nullptr;
    }

    errors.emplace_back(currentToken.line, currentToken.column,
                        "Invalid statement");
    advance();
    return nullptr;
}

unique_ptr<ASTNode> Parser::parseFuncCall() {
    auto funcNameToken = currentToken;
    expect(TokenType::Identifier, "Expected function name");

    if (!match(TokenType::Delimiter, "(")) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Expected '(' after function name");
        return nullptr;
    }

    vector<unique_ptr<ASTNode>> args;

    if (!match(TokenType::Delimiter, ")")) {
        do {
            auto arg = parseExpr();
            if (!arg) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected expression in function arguments");
                break;
            }
            args.push_back(move(arg));
        } while (match(TokenType::Delimiter, ","));

        if (!match(TokenType::Delimiter, ")")) {
            errors.emplace_back(currentToken.line, currentToken.column,
                                "Expected ')' after function arguments");
            return nullptr;
        }
    }

    return make_unique<CallNode>(funcNameToken,
                                 make_unique<IdentifierNode>(funcNameToken),
                                 move(args));
}

unique_ptr<ASTNode> Parser::parseAssignStmt() {
    auto idToken = currentToken;
    expect(TokenType::Identifier, "Expected identifier for assignment");

    if (currentToken.type == TokenType::Operator && currentToken.value == "=") {
        expect(TokenType::Operator, "Expected '=' in assignment", "=");
    } else {
        expect(TokenType::Assignment, "Expected '=' in assignment", "=");
    }

    auto expr = parseExpr();
    if (!expr) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Expected expression after '='");
        return nullptr;
    }

    // Get the evaluated value and type
    string value = evaluateExpression(expr.get());
    string type = getTypeFromNode(expr); // Use the unified type detection

    if(type == "unknown")
        type = "expr";
    // Update symbol table with the variable information
    symbolTable.declare(idToken.value, type, "variable", value);

    return make_unique<AssignNode>(idToken,
                                   make_unique<IdentifierNode>(idToken),
                                   move(expr));
}
unique_ptr<ASTNode> Parser::parseReturnStmt() {
    auto returnToken = currentToken;
    expect(TokenType::Keyword, "Expected 'return' keyword", "return");

    auto expr = parseExpr();
    string returnValue = expr ? getValueFromNode(expr.get()) : "void";
    string returnType = expr ? getTypeFromNode(expr) : "void";

    // Extract function name from current scope
    string currentScope = symbolTable.getCurrentScope();
    size_t funcSuffixPos = currentScope.find(" (function)");
    if (funcSuffixPos != string::npos) {
        string functionName = currentScope.substr(0, funcSuffixPos);

        // Update function's return type and value in symbol table
        auto entry = symbolTable.lookupEntry(functionName);
        if (entry && entry->role == "function") {
            symbolTable.updateType(functionName, returnType);
            symbolTable.updateValue(functionName, returnValue);
        }
    }

    return make_unique<ReturnNode>(returnToken, expr ? move(expr) : nullptr);
}

unique_ptr<ASTNode> Parser::parseIfStmt() {
    auto ifToken = currentToken;
    expect(TokenType::Keyword, "Expected 'if' keyword", "if");

    auto condition = parseExpr();
    expect(TokenType::Delimiter, "Expected ':' after if condition", ":");

    // Save the current indentation level before entering the if block
    int ifIndent = indentStack.empty() ? 0 : indentStack.back();
    int blockIndent = ifIndent + 1;
    indentStack.push_back(blockIndent);

    expectIndent(blockIndent);

    symbolTable.beginScope("if block");

    auto thenBlock = make_unique<BlockNode>(currentToken);
    while (currentToken.type != TokenType::EOFToken &&
           (currentToken.type != TokenType::Indent ||
            stoi(currentToken.value) >= blockIndent)) {
        if (currentToken.type == TokenType::Indent) {
            expectIndent(blockIndent);
            continue;
        }
        auto stmt = parseStmt();

        if (stmt) {
            thenBlock->addChild(move(stmt));
        }
        else {
            break;
        }
    }
    symbolTable.endScope();
    indentStack.pop_back();

    // IMPORTANT: Only process elif/else at the same level as the original if
    vector<unique_ptr<ElifNode>> elifBranches;
    while (true) {
        // Check for correct indentation level before processing elif
        if (currentToken.type == TokenType::Indent) {
            int currentIndent = stoi(currentToken.value);
            if (currentIndent != ifIndent) break;
            advance(); // Consume the indent
        }

        if (currentToken.type != TokenType::Keyword || currentToken.value != "elif") {
            break;
        }

        auto elifToken = currentToken;
        expect(TokenType::Keyword, "Expected 'elif' keyword", "elif");

        auto elifCondition = parseExpr();
        expect(TokenType::Delimiter, "Expected ':' after elif condition", ":");

        int elifBlockIndent = ifIndent + 1;
        indentStack.push_back(elifBlockIndent);

        expectIndent(elifBlockIndent);

        symbolTable.beginScope("elif block");

        auto elifBlock = make_unique<BlockNode>(currentToken);
        while (currentToken.type != TokenType::EOFToken &&
               (currentToken.type != TokenType::Indent ||
                stoi(currentToken.value) >= elifBlockIndent)) {
            if (currentToken.type == TokenType::Indent) {
                expectIndent(elifBlockIndent);
                continue;
            }
            auto stmt = parseStmt();

            if (stmt) {
                elifBlock->addChild(move(stmt));
            }
        }

        symbolTable.endScope();
        indentStack.pop_back();

        elifBranches.push_back(make_unique<ElifNode>(elifToken, move(elifCondition), move(elifBlock)));
    }

    // Check for else at the same level as the original if
    unique_ptr<ASTNode> elseBlock = nullptr;
    if (currentToken.type == TokenType::Indent) {
        int currentIndent = stoi(currentToken.value);
        if (currentIndent == ifIndent) {
            advance(); // Consume the indent
            if (currentToken.type == TokenType::Keyword && currentToken.value == "else") {
                elseBlock = parseElseStmt();
            }
        }
    }
    else if (currentToken.type == TokenType::Keyword && currentToken.value == "else") {
        elseBlock = parseElseStmt();
    }

    return make_unique<IfNode>(ifToken, move(condition), move(thenBlock), move(elseBlock), move(elifBranches));
}

unique_ptr<ASTNode> Parser::parseElseStmt() {
    auto elseToken = currentToken;
    expect(TokenType::Keyword, "Expected 'else' keyword", "else");
    expect(TokenType::Delimiter, "Expected ':' after else", ":");

    int currentIndent = indentStack.empty() ? 0 : indentStack.back();
    int blockIndent = currentIndent + 1;
    indentStack.push_back(blockIndent);

    expectIndent(blockIndent);

    symbolTable.beginScope("else block");

    auto elseBlock = make_unique<BlockNode>(currentToken);
    while (currentToken.type != TokenType::EOFToken &&
           (currentToken.type != TokenType::Indent ||
            stoi(currentToken.value) >= blockIndent)) {
        if (currentToken.type == TokenType::Indent) {
            expectIndent(blockIndent);
            continue;
        }
        auto stmt = parseStmt();
        if (stmt) {
            elseBlock->addChild(move(stmt));
        }
    }

    symbolTable.endScope();
    indentStack.pop_back();

    return elseBlock;
}

unique_ptr<ASTNode> Parser::parseForStmt() {
    auto forToken = currentToken;
    expect(TokenType::Keyword, "Expected 'for' keyword", "for");

    // Parse loop variable
    auto var = parsePrimary(); // Should be an identifier
    if (!var || !dynamic_cast<IdentifierNode*>(var.get())) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Expected identifier after 'for'");
        return nullptr;
    }

    expect(TokenType::Keyword, "Expected 'in' after loop variable", "in");

    // Parse iterable expression
    auto iterable = parseExpr();
    if (!iterable) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Expected iterable expression after 'in'");
        return nullptr;
    }

    expect(TokenType::Delimiter, "Expected ':' after for loop header", ":");

    // Handle indentation and block
    int currentIndent = indentStack.empty() ? 0 : indentStack.back();
    int blockIndent = currentIndent + 1;
    indentStack.push_back(blockIndent);

    expectIndent(blockIndent);

    symbolTable.beginScope("for loop");

    auto body = make_unique<BlockNode>(currentToken);
    while (currentToken.type != TokenType::EOFToken &&
           (currentToken.type != TokenType::Indent ||
            stoi(currentToken.value) >= blockIndent)) {
        if (currentToken.type == TokenType::Indent) {
            expectIndent(blockIndent);
            continue;
        }
        auto stmt = parseStmt();
        if (stmt) body->addChild(move(stmt));
    }

    symbolTable.endScope();
    indentStack.pop_back();

    return make_unique<ForNode>(forToken, move(var), move(iterable), move(body));
}

unique_ptr<ASTNode> Parser::parseWhileStmt() {
    auto whileToken = currentToken;
    expect(TokenType::Keyword, "Expected 'while' keyword", "while");

    auto condition = parseExpr();
    expect(TokenType::Delimiter, "Expected ':' after while condition", ":");

    // Get current indentation level
    int currentIndent = indentStack.empty() ? 0 : indentStack.back();
    int blockIndent = currentIndent + 1;
    indentStack.push_back(blockIndent);

    // Expect indentation for the block
    expectIndent(blockIndent);

    // Enter new scope for while block
    symbolTable.beginScope("while block");

    auto body = make_unique<BlockNode>(currentToken);
    while (currentToken.type != TokenType::EOFToken &&
           (currentToken.type != TokenType::Indent ||
            stoi(currentToken.value) >= blockIndent)) {
        if (currentToken.type == TokenType::Indent) {
            expectIndent(blockIndent);
            continue;
        }
        auto stmt = parseStmt();
        if (stmt) body->addChild(move(stmt));
    }

    // Exit while block scope
    symbolTable.endScope();
    indentStack.pop_back();

    return make_unique<WhileNode>(whileToken, move(condition), move(body));
}

unique_ptr<ASTNode> Parser::parseFuncDef() {
    auto defToken = currentToken;
    expect(TokenType::Keyword, "Expected 'def' keyword", "def");

    auto funcNameToken = currentToken;
    expect(TokenType::Identifier, "Expected function name after 'def'");

    // Add function to symbol table with unknown return value initially
    symbolTable.declare(funcNameToken.value, "function", "function", "unknown");

    // Enter new scope for function
    symbolTable.beginScope(funcNameToken.value + " (function)");

    expect(TokenType::Delimiter, "Expected '(' after function name", "(");

    vector<string> paramNames;
    if (currentToken.type == TokenType::Identifier) {
        // Add parameter to symbol table
        symbolTable.declare(currentToken.value, "unknown", "parameter");
        paramNames.push_back(currentToken.value);
        advance();

        while (match(TokenType::Delimiter, ",")) {
            if (currentToken.type != TokenType::Identifier) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected parameter name after ','");
                break;
            }
            // Add parameter to symbol table
            symbolTable.declare(currentToken.value, "unknown", "parameter");
            paramNames.push_back(currentToken.value);
            advance();
        }
    }

    expect(TokenType::Delimiter, "Expected ')' after parameter list", ")");
    expect(TokenType::Delimiter, "Expected ':' after function definition", ":");

    // Get current indentation level
    int currentIndent = indentStack.empty() ? 0 : indentStack.back();
    int blockIndent = currentIndent + 1;
    indentStack.push_back(blockIndent);

    // Expect indentation for the block
    expectIndent(blockIndent);

    auto body = make_unique<BlockNode>(currentToken);
    while (currentToken.type != TokenType::EOFToken &&
           (currentToken.type != TokenType::Indent ||
            stoi(currentToken.value) >= blockIndent)) {
        if (currentToken.type == TokenType::Indent) {
            expectIndent(blockIndent);
            continue;
        }
        auto stmt = parseStmt();
        if (stmt) body->addChild(move(stmt));
    }

    // Exit function scope
    symbolTable.endScope();
    indentStack.pop_back();

    return make_unique<FunctionDefNode>(defToken, funcNameToken.value, move(paramNames), move(body));
}

unique_ptr<ASTNode> Parser::parseFuncCallStmt() {
    auto funcNameToken = currentToken;
    expect(TokenType::Identifier, "Expected identifier for function call");

    expect(TokenType::Delimiter, "Expected '(' in function call", "(");

    vector<unique_ptr<ASTNode>> args;
    if (!match(TokenType::Delimiter, ")")) {
        do {
            auto arg = parseExpr();
            if (!arg) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected expression in function arguments");
                break;
            }
            args.push_back(move(arg));
        } while (match(TokenType::Delimiter, ","));

        expect(TokenType::Delimiter, "Expected ')' after arguments", ")");
    }

    // Look up function return type if it exists
    string returnType = "unknown";
    auto funcEntry = symbolTable.lookupEntry(funcNameToken.value);
    if (funcEntry && funcEntry->role == "function") {
        returnType = funcEntry->dataType;
    } else {
        symbolTable.declare(funcNameToken.value, "unknown", "function", "unknown");
    }

    return make_unique<CallNode>(funcNameToken,
                                 make_unique<IdentifierNode>(funcNameToken),
                                 move(args));
}

unique_ptr<ASTNode> Parser::parseExpr() {
    if (currentToken.type == TokenType::EOFToken) {
        errors.emplace_back(currentToken.line, currentToken.column,
                            "Unexpected end of file while parsing expression");
        return nullptr;
    }
    return parseLogicalOr();
}


unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto node = parseLogicalAnd();
    if (!node) return nullptr;

    Token op = currentToken;
    while (match(TokenType::Keyword, "or")) {
        auto right = parseLogicalAnd();
        if (!right) {
            errors.emplace_back(currentToken.line, currentToken.column,
                                "Expected right-hand expression after 'or'");
            return node;  // Return what we have so far
        }
        node = make_unique<BinaryOpNode>(op, move(node), move(right));
    }
    return node;
}

unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto node = parseEquality();
    if (!node) return nullptr;
    Token op = currentToken;
    while (match(TokenType::Keyword, "and")) {
        cout<<"hhh"<<endl;
        auto right = parseEquality();
        if (!right) {
            errors.emplace_back(currentToken.line, currentToken.column,
                                "Expected right-hand expression after 'and'");
            return node;  // Return what we have so far
        }
        node = make_unique<BinaryOpNode>(op, move(node), move(right));
    }
    return node;
}
unique_ptr<ASTNode> Parser::parseEquality() {
    auto node = parseRelational();
    if (!node) return nullptr;

    while (true) {
        Token op = currentToken;
        if (match(TokenType::Operator, "==") || match(TokenType::Operator, "!=")) {
            //Token op = currentToken;
            auto right = parseRelational();
            if (!right) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected right-hand expression after operator");
                return nullptr;
            }
            node = make_unique<BinaryOpNode>(op, move(node), move(right));
        } else {
            break;
        }
    }
    return node;
}

unique_ptr<ASTNode> Parser::parseRelational() {
    auto node = parseTerm();
    if (!node) return nullptr;

    while (true) {
        Token op = currentToken;
        if (match(TokenType::Operator, "<") || match(TokenType::Operator, ">") ||
            match(TokenType::Operator, "<=") || match(TokenType::Operator, ">=")) {
            //Token op = currentToken;
            auto right = parseTerm();
            if (!right) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected right-hand expression after comparison");
                return nullptr;
            }
            node = make_unique<BinaryOpNode>(op, move(node), move(right));
        } else {
            break;
        }
    }
    return node;
}

unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();
    if (!node) return nullptr;

    while (true) {
        Token op = currentToken;
        if (match(TokenType::Operator, "+") || match(TokenType::Operator, "-")) {
            //Token op = currentToken;
            auto right = parseFactor();
            if (!right) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected right-hand expression after operator");
                return nullptr;
            }
            node = make_unique<BinaryOpNode>(op, move(node), move(right));
        } else {
            break;
        }
    }
    return node;
}

unique_ptr<ASTNode> Parser::parseFactor() {
    auto node = parseUnary();
    if (!node) return nullptr;

    while (true) {
        Token op = currentToken;
        if (match(TokenType::Operator, "*") || match(TokenType::Operator, "/") ||
            match(TokenType::Operator, "%")) {
            //Token op = currentToken;
            auto right = parseUnary();
            if (!right) {
                errors.emplace_back(currentToken.line, currentToken.column,
                                    "Expected right-hand expression after operator");
                return nullptr;
            }
            node = make_unique<BinaryOpNode>(op, move(node), move(right));
        } else {
            break;
        }
    }
    return node;
}

unique_ptr<ASTNode> Parser::parseUnary() {
    Token op = currentToken;
    if (match(TokenType::Operator, "-") || match(TokenType::Keyword, "not")) {
        //Token op = currentToken;
        auto right = parseUnary();
        if (!right) {
            errors.emplace_back(currentToken.line, currentToken.column,
                                "Expected expression after unary operator");
            return nullptr;
        }
        return make_unique<UnaryOpNode>(op, move(right));
    }
    return parsePrimary();
}

unique_ptr<ASTNode> Parser::parsePrimary() {
    if (currentToken.type == TokenType::Number) {
        Token numToken = currentToken;
        advance();
        try {
            return make_unique<NumberNode>(numToken);
        } catch (...) {
            errors.emplace_back(numToken.line, numToken.column,
                                "Invalid number format");
            return nullptr;
        }
    }
    if (currentToken.type == TokenType::String) {
        Token strToken = currentToken;
        advance();
        return make_unique<StringNode>(strToken);
    }
    if (currentToken.type == TokenType::Keyword &&
        (currentToken.value == "True" || currentToken.value == "False")) {
        Token boolToken = currentToken;
        advance();
        return make_unique<BooleanNode>(boolToken);
    }
    if (currentToken.type == TokenType::Identifier) {
        if (peekNextToken().value == "(") {
            return parseFuncCall();
        }
        Token idToken = currentToken;
        advance();
        return make_unique<IdentifierNode>(idToken);
    }
    if (match(TokenType::Delimiter, "(")) {
        auto expr = parseExpr();
        expect(TokenType::Delimiter, "Expected ')' after expression", ")");
        return expr;
    }

    errors.emplace_back(currentToken.line, currentToken.column,
                        "Unexpected token in expression");
    advance();
    return nullptr;
}

void Parser::addError(int line, int col, const string &msg) {
    errors.emplace_back(line, col, msg);
}

void Parser::printErrors() {
    if (errors.empty()) {
        std::cout << "No errors found\n";
        return;
    }

    std::cout << "Found " << errors.size() << " errors:\n";
    for (const auto& err : errors) {
        std::cout << "* Line " << err.line
                  << ", Col " << err.col
                  << ": " << err.message << "\n";
    }
}

string Parser::getValueFromNode(ASTNode* node) {
    if (!node) return "unknown";

    if (auto numNode = dynamic_cast<NumberNode*>(node)) {
        return to_string(numNode->getValue());
    } else if (auto strNode = dynamic_cast<StringNode*>(node)) {
        return strNode->getToken().value;
    } else if (auto boolNode = dynamic_cast<BooleanNode*>(node)) {
        return boolNode->getToken().value;
    } else if (auto idNode = dynamic_cast<IdentifierNode*>(node)) {
        return idNode->getToken().value;
    } else if (auto callNode = dynamic_cast<CallNode*>(node)) {
        // Look up function return type
        auto entry = symbolTable.lookupEntry(callNode->getToken().value);
        return (entry && entry->role == "function") ? entry->value : "unknown";
    }
    return "unknown";
}

string Parser::getTypeFromNode(const unique_ptr<ASTNode>& node) {
    if (!node) return "unknown";

    if (auto numNode = dynamic_cast<NumberNode*>(node.get())) {
        return numNode->getValueType();
    }
    else if (dynamic_cast<StringNode*>(node.get())) {
        return "string";
    }
    else if (dynamic_cast<BooleanNode*>(node.get())) {
        return "boolean";
    }
    else if (dynamic_cast<IdentifierNode*>(node.get())) {
        // Look up identifier type in symbol table
        auto entry = symbolTable.lookupEntry(node->getToken().value);
        return entry ? entry->dataType : "unknown";
    }
    else if (auto callNode = dynamic_cast<CallNode*>(node.get())) {
        // Look up function return type
        auto entry = symbolTable.lookupEntry(callNode->getToken().value);
        return (entry && entry->role == "function") ? entry->dataType : "unknown";
    }
    return "unknown";
}

string Parser::evaluateExpression(ASTNode* node) {
    if (!node) return "unknown";

    // Handle binary operations
    if (auto binOp = dynamic_cast<BinaryOpNode*>(node)) {
        string left = evaluateExpression(binOp->getLeft());
        string right = evaluateExpression(binOp->getRight());
        string op = binOp->getOp().value;

        // Handle logical operators first
        if (op == "and") {
            bool leftBool = (left == "True" || left == "1");
            bool rightBool = (right == "True" || right == "1");
            return (leftBool && rightBool) ? "True" : "False";
        }
        else if (op == "or") {
            bool leftBool = (left == "True" || left == "1");
            bool rightBool = (right == "True" || right == "1");
            return (leftBool || rightBool) ? "True" : "False";
        }

        // Then handle arithmetic operators
        try {
            double leftNum = stod(left);
            double rightNum = stod(right);

            if (op == "+") return to_string(leftNum + rightNum);
            if (op == "-") return to_string(leftNum - rightNum);
            if (op == "*") return to_string(leftNum * rightNum);
            if (op == "/") {
                if (rightNum == 0) return "DivisionByZeroError";
                return to_string(leftNum / rightNum);
            }
            if (op == "%") return to_string((int)leftNum % (int)rightNum);
            if (op == "==") return (leftNum == rightNum) ? "True" : "False";
            if (op == "!=") return (leftNum != rightNum) ? "True" : "False";
            if (op == "<") return (leftNum < rightNum) ? "True" : "False";
            if (op == ">") return (leftNum > rightNum) ? "True" : "False";
            if (op == "<=") return (leftNum <= rightNum) ? "True" : "False";
            if (op == ">=") return (leftNum >= rightNum) ? "True" : "False";
        } catch (...) {
            // If conversion fails, return symbolic expression
            return left + " " + op + " " + right;
        }
    }
    // Handle unary operations
    else if (auto unOp = dynamic_cast<UnaryOpNode*>(node)) {
        string operand = evaluateExpression(unOp->getOperand());
        string op = unOp->getOp().value;

        if (op == "not") {
            bool operandBool = (operand == "True" || operand == "1");
            return (!operandBool) ? "True" : "False";
        }

        try {
            double num = stod(operand);
            if (op == "-") return to_string(-num);
            if (op == "+") return operand; // Unary plus doesn't change value
        } catch (...) {
            return op + " " + operand;
        }
    }
    // Handle identifiers by looking up their value in symbol table
    else if (auto idNode = dynamic_cast<IdentifierNode*>(node)) {
        auto entry = symbolTable.lookupEntry(idNode->getToken().value);
        if (entry) {
            // Convert common boolean representations
            if (entry->value == "true") return "True";
            if (entry->value == "false") return "False";
            return entry->value;
        }
        return idNode->getToken().value;
    }
    // Handle boolean literals directly
    else if (auto boolNode = dynamic_cast<BooleanNode*>(node)) {
        return boolNode->getToken().value;
    }

    // For other nodes, just return their value
    return getValueFromNode(node);
}

