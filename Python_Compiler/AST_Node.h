#ifndef AST_NODE_H
#define AST_NODE_H

#include <vector>
#include <memory>
#include <string>
#include <qDebug>
using namespace std;

// Token types recognized by the lexer
enum class TokenType {
    Keyword, Identifier, Number, String, Operator, Delimiter,
    Assignment, Boolean, Arithmetic, EOFToken, Error, Indent
};

// Represents a single token
struct Token {
    TokenType type;
    string value;
    int line, column;
    int symbolId = -1;  // Only used for identifiers
};

// Abstract base class for all AST nodes
class ASTNode {
public:
    Token token;
    std::vector<std::unique_ptr<ASTNode>> children;

    ASTNode(const Token& t) : token(t) {}
    virtual ~ASTNode() = default;

    virtual std::string getNodeType() const = 0;

    virtual std::string toString(int indent = 0, const std::string& prefix = "", bool isLast = true) const {
        std::string result;
        result += prefix + (isLast ? "└──" : "├──");
        result += getNodeType() + " [" + token.value + "] (line " + std::to_string(token.line) + ")\n";

        std::string childPrefix = prefix + (isLast ? "    " : "│    ");
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->toString(indent + 1, childPrefix, i == children.size() - 1);
        }

        return result;
    }

    virtual std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const {
        std::string result = prefix + (isLast ? "└──" : "├──") + getNodeType();

        // Only show token value if it's not empty (for nodes that use it)
        if (!token.value.empty() && token.value != getNodeType()) {
            result += " [" + token.value + "]";
        }
        result += "\n";

        std::string childPrefix = prefix + (isLast ? "    " : "│    ");
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->toParseTreeString(indent + 1, childPrefix, i == children.size() - 1);
        }

        return result;
    }

    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }

    Token getToken(){
        return token;
    }

    virtual ASTNode* getLeft() const { return nullptr; }
    virtual ASTNode* getRight() const { return nullptr; }
    virtual ASTNode* getOperand() const { return nullptr; }
    virtual Token getOp() const { return Token{TokenType::Error, "", 0, 0}; }
};

// Program root node
class ProgramNode : public ASTNode {
public:
    ProgramNode() : ASTNode({TokenType::EOFToken, "Program", 1, 1}) {}

    std::string getNodeType() const override { return "Program"; }

    std::string toString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + getNodeType() + "\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->toString(indent + 1, childPrefix, i == children.size() - 1);
        }
        return result;
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "Program\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->toParseTreeString(indent + 1, childPrefix, i == children.size() - 1);
        }
        return result;
    }
};

// Expression nodes
class BinaryOpNode : public ASTNode {
public:
    BinaryOpNode(const Token& op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : ASTNode(op) {
        addChild(std::move(left));
        addChild(std::move(right));
    }

    std::string getNodeType() const override { return "BinaryOp"; }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "BinaryOp\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Left child
        result += children[0]->toParseTreeString(indent + 1, childPrefix, false);

        // Operator
        result += childPrefix + "├── " + token.value + "\n";

        // Right child
        result += children[1]->toParseTreeString(indent + 1, childPrefix, true);

        return result;
    }

    ASTNode* getLeft() const override { return children.size() > 0 ? children[0].get() : nullptr; }
    ASTNode* getRight() const override { return children.size() > 1 ? children[1].get() : nullptr; }
    Token getOp() const override { return token; }
};

class UnaryOpNode : public ASTNode {
public:
    UnaryOpNode(const Token& op, std::unique_ptr<ASTNode> operand)
        : ASTNode(op) {
        addChild(std::move(operand));
    }

    std::string getNodeType() const override { return "UnaryOp"; }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "UnaryOp [" + token.value + "]\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");
        result += children[0]->toParseTreeString(indent + 1, childPrefix, true);
        return result;
    }

    ASTNode* getOperand() const override { return children.size() > 0 ? children[0].get() : nullptr; }
    Token getOp() const override { return token; }
};

class NumberNode : public ASTNode {
public:
    double value;
    string type;
    NumberNode(const Token& numToken)
        : ASTNode(numToken), value(std::stod(numToken.value)) {
        if(numToken.value.find(".")  == string::npos){
            type = "int";
        }
        else type = "float";
    }

    std::string getNodeType() const override { return "Number"; }

    double getValue(){
        return value;
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        return prefix + (isLast ? "└──" : "├──") + "Number [" + token.value + "]\n";
    }

    string getValueType() const {
        return type;
    }
};

class StringNode : public ASTNode {
public:
    StringNode(const Token& strToken)
        : ASTNode(strToken) {}

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        return prefix + (isLast ? "└──" : "├──") + "String [" + token.value + "]\n";
    }

    std::string getNodeType() const override { return "String"; }
};

class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const Token& idToken)
        : ASTNode(idToken) {}

    std::string getNodeType() const override { return "Identifier"; }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        return prefix + (isLast ? "└──" : "├──") + "Identifier [" + token.value + "]\n";
    }
};

// Statement nodes
class AssignNode : public ASTNode {
public:
    AssignNode(const Token& assignToken, std::unique_ptr<ASTNode> target, std::unique_ptr<ASTNode> value)
        : ASTNode(assignToken) {
        addChild(std::move(target));
        addChild(std::move(value));
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "Assignment\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Target
        result += children[0]->toParseTreeString(indent + 1, childPrefix, false);

        // Operator
        result += childPrefix + "├── Operator [=]\n";

        // Value
        result += children[1]->toParseTreeString(indent + 1, childPrefix, true);

        return result;
    }

    std::string getNodeType() const override { return "Assign"; }
};

// New node type for elif conditions
class ElifNode : public ASTNode {
public:
    ElifNode(const Token& token, std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> thenBranch)
        : ASTNode(token) {
        addChild(std::move(condition));
        addChild(std::move(thenBranch));
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "Elif\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Condition
        result += children[0]->toParseTreeString(indent + 1, childPrefix, false);

        // Then branch
        result += children[1]->toParseTreeString(indent + 1, childPrefix, true);

        return result;
    }

    std::string getNodeType() const override { return "Elif"; }

    ASTNode* getCondition() const { return children[0].get(); }
    ASTNode* getThenBranch() const { return children[1].get(); }
};

class IfNode : public ASTNode {
public:
    IfNode(const Token& token, std::unique_ptr<ASTNode> condition,
           std::unique_ptr<ASTNode> thenBranch,
           std::unique_ptr<ASTNode> elseBranch = nullptr,
           std::vector<std::unique_ptr<ElifNode>> elifBranches = {})
        : ASTNode(token) {
        addChild(std::move(condition));   // index 0
        addChild(std::move(thenBranch));  // index 1
        // Add elif branches as children after else
        for (auto& elif : elifBranches) {
            addChild(std::move(elif));
        }
        if (elseBranch) addChild(std::move(elseBranch)); // last index (optional)


    }

    std::string getNodeType() const override { return "If"; }

    ASTNode* getCondition() const { return children[0].get(); }
    ASTNode* getThenBranch() const { return children[1].get(); }
    ASTNode* getElseBranch() const {
        // Else branch is at index 2 if it exists, but after that come elif branches
        int idx= children.size() -1;
        return children.size() > 2 && children[idx]->getNodeType() == "Else" ? children[idx].get() : nullptr;
    }

    std::vector<ElifNode*> getElifBranches() const {
        std::vector<ElifNode*> elifs;
        for (size_t i = 2; i < children.size(); ++i) {
            if (children[i]->getNodeType() == "Elif") {
                elifs.push_back(static_cast<ElifNode*>(children[i].get()));
            }
        }
        return elifs;
    }


    std::string toString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + getNodeType() + "\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Print condition
        result += children[0]->toString(indent + 1, childPrefix, false);

        // Print then branch
        result += children[1]->toString(indent + 1, childPrefix, children.size() == 2);

        // Elif branches
        size_t idx = 2;
        for (; idx < children.size(); ++idx) {
            if (children[idx]->getNodeType() == "Elif") {
                result += childPrefix + "├──" + children[idx]->getNodeType() + ":";
                result += children[idx]->toString(indent + 1, childPrefix + "│    ",
                                                  // if last among elif and no else
                                                  idx + 1 == children.size());
            } else {
                break;
            }
        }

        // Else branch (if exists)
        if (idx < children.size()) {
            result += childPrefix + "├──" + children[idx]->getNodeType() + ":";
            result += children[idx]->toString(indent + 1, childPrefix + "│    ", true);
        }

        return result;
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "If\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Condition
        result += children[0]->toParseTreeString(indent + 1, childPrefix, false);

        // Then branch
        if(children.size() <= 2){
            result += childPrefix + "└── Then\n";
            result += children[1]->toParseTreeString(indent + 1, childPrefix + "    ", true);
        }
        else{
            result += childPrefix + "├── Then\n";
            result += children[1]->toParseTreeString(indent + 1, childPrefix + "│    ", true);
        }

        // Elif branches
        size_t idx = 2;
        for (; idx < children.size(); ++idx) {
            if (children[idx]->getNodeType() == "Elif") {
                result += children[idx]->toParseTreeString(indent + 1, childPrefix, idx + 1 == children.size());
            } else {
                break;
            }
        }

        // Else branch (if exists)
        if (idx < children.size()) {
            result += childPrefix + "└── Else\n";
            result += children[idx]->toParseTreeString(indent + 1, childPrefix + "    ", true);
        }

        return result;
    }
};

class ForNode : public ASTNode {
public:
    ForNode(const Token& forToken,
            std::unique_ptr<ASTNode> var,
            std::unique_ptr<ASTNode> iterable,
            std::unique_ptr<ASTNode> body)
        : ASTNode(forToken) {
        addChild(std::move(var));       // index 0 - loop variable
        addChild(std::move(iterable));  // index 1 - iterable expression
        addChild(std::move(body));      // index 2 - loop body
    }

    std::string getNodeType() const override { return "For"; }

    ASTNode* getVariable() const { return children[0].get(); }
    ASTNode* getIterable() const { return children[1].get(); }
    ASTNode* getBody() const { return children[2].get(); }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "For\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Variable
        result += childPrefix + "├── Variable\n";
        result += children[0]->toParseTreeString(indent + 1, childPrefix + "│    ", false);

        // Iterable
        result += childPrefix + "├── Iterable\n";
        result += children[1]->toParseTreeString(indent + 1, childPrefix + "│    ", false);

        // Body
        result += childPrefix + "└── Body\n";
        result += children[2]->toParseTreeString(indent + 1, childPrefix + "    ", true);

        return result;
    }
};

class WhileNode : public ASTNode {
public:
    WhileNode(const Token& whileToken, std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> body)
        : ASTNode(whileToken) {
        addChild(std::move(condition));
        addChild(std::move(body));
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "While\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Condition
        result += children[0]->toParseTreeString(indent + 1, childPrefix, false);

        // Body
        result += childPrefix + "└── Body\n";
        result += children[1]->toParseTreeString(indent + 1, childPrefix + "    ", true);

        return result;
    }

    std::string getNodeType() const override { return "While"; }
};

class FunctionDefNode : public ASTNode {
public:
    std::vector<std::string> paramNames;

    FunctionDefNode(const Token& defToken, const std::string& name,
                    std::vector<std::string> params, std::unique_ptr<ASTNode> body)
        : ASTNode(defToken), paramNames(std::move(params)) {
        addChild(std::make_unique<IdentifierNode>(Token{TokenType::Identifier, name, defToken.line, defToken.column}));
        addChild(std::move(body));
    }

    std::string getNodeType() const override { return "FunctionDef"; }

    std::string toString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──");
        result += getNodeType() + " " + children[0]->token.value + "\n";

        std::string paramPrefix = prefix + (isLast ? "    " : "│    ");
        result += paramPrefix + "├── Parameters: ";
        for (size_t i = 0; i < paramNames.size(); ++i) {
            result += paramNames[i];
            if (i < paramNames.size() - 1) result += ", ";
        }
        result += "\n";

        result += children[1]->toString(indent + 1, paramPrefix, true);
        return result;
    }
    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "FunctionDef [" + children[0]->token.value + "]\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        // Parameters
        result += childPrefix + "├── Parameters\n";
        std::string paramPrefix = childPrefix + "│    ";
        for (size_t i = 0; i < paramNames.size(); ++i) {
            bool lastParam = (i == paramNames.size() - 1);
            result += paramPrefix + (lastParam ? "└──" : "├──") + "Parameter [" + paramNames[i] + "]\n";
        }

        // Body
        result += childPrefix + "└── Body\n";
        result += children[1]->toParseTreeString(indent + 1, childPrefix + "    ", true);

        return result;
    }
};

class ReturnNode : public ASTNode {
public:
    ReturnNode(const Token& returnToken, std::unique_ptr<ASTNode> value = nullptr)
        : ASTNode(returnToken) {
        if (value) {
            addChild(std::move(value));
        }
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "Return\n";
        if (!children.empty()) {
            std::string childPrefix = prefix + (isLast ? "    " : "│    ");
            result += children[0]->toParseTreeString(indent + 1, childPrefix, true);
        }
        return result;
    }

    std::string getNodeType() const override { return "Return"; }
};

class BlockNode : public ASTNode {
public:
    BlockNode(const Token& blockToken)
        : ASTNode(blockToken) {}

    std::string getNodeType() const override {
        if(token.value == "else")
            return "Else";
        else
            return "Block"; }

    std::string toString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + getNodeType() + "\n";
        std::string childPrefix = prefix + (isLast ? "    " : "  ");
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->toString(indent + 1, childPrefix, i == children.size() - 1);
        }
        return result;
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + getNodeType() + "\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->toParseTreeString(indent + 1, childPrefix, i == children.size() - 1);
        }
        return result;
    }
};

class CallNode : public ASTNode {
public:
    CallNode(const Token& callToken, std::unique_ptr<ASTNode> func,
             std::vector<std::unique_ptr<ASTNode>> args)
        : ASTNode(callToken) {
        addChild(std::move(func));
        for (auto& arg : args) {
            addChild(std::move(arg));
        }
    }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        std::string result = prefix + (isLast ? "└──" : "├──") + "Call: " + children[0]->token.value + "\n";
        std::string childPrefix = prefix + (isLast ? "    " : "│    ");

        if (children.size() > 1) {
            result += childPrefix + "├── (\n";
            result += childPrefix + "│    ├── Arguments\n";
            std::string argPrefix = childPrefix + "│    │    ";
            for (size_t i = 1; i < children.size(); ++i) {
                result += children[i]->toParseTreeString(indent + 1, argPrefix, i == children.size() - 1);
            }
            result += childPrefix + "└── )\n";
        }

        return result;
    }

    std::string getNodeType() const override { return "Call"; }
};

class BooleanNode : public ASTNode {
public:
    BooleanNode(const Token& boolToken) : ASTNode(boolToken) {}
    string getNodeType() const override { return "Boolean"; }

    std::string toParseTreeString(int indent = 0, const std::string& prefix = "", bool isLast = true) const override {
        return prefix + (isLast ? "└──" : "├──") + "Boolean [" + token.value + "]\n";
    }
};

#endif // AST_NODE_H
