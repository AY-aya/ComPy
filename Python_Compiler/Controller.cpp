#include "Controller.h"
#include "ParserSymbolTable.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

QJsonObject astNodeToJson(const ASTNode* node);

Controller::Controller(QObject *parent)
    : QObject{parent}
{}


void Controller::loadFile(const QString &path) {
    string fileN =path.toStdString();
    fileN = fileN.substr(8, fileN.length());
    QFile file(QString::fromStdString(fileN));

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_code = in.readAll();
        emit codeChanged();

        m_lexer = std::make_unique<Lexer>(m_code.toStdString());
    }
}

void Controller::clearCode()
{
    m_code.clear();
    m_symbolTable.clear();
    m_parserSymbolTable.clear();
    m_tokens.clear();
    m_parserErrors.clear();
    m_errors.clear();
    m_parseTreeJson.clear();
    emit codeChanged();
    emit symbolTableChanged();
    emit parserSymbolTableChanged();
    emit errorsChanged();
    emit parserErrorsChanged();
    emit tokensChanged();
    emit parseTreeJsonChanged();
}

void Controller::runLexer() {
    if (!m_lexer) return;
    auto tokens = m_lexer->tokenize();
    lexer_tokens = tokens;

    m_tokens.clear();
    m_errors.clear();

    int count = 1;
    int errCount = 0;
    for (const auto &token : tokens) {
        if (token.type == TokenType::EOFToken) break;
        //if (token.type == TokenType::Error) continue;  //Skip error tokens
        if (token.type == TokenType::Indent) continue;  //Skip indent tokens

        QString tokenStr = QString::number(count++) + ". <";
        QString errorStr = "";

        switch (token.type) {
        case TokenType::Identifier:
            tokenStr += "identifier, " + QString::number(token.symbolId);
            break;
        case TokenType::Keyword:
            tokenStr += "keyword, " + QString::fromStdString(token.value);
            break;
        case TokenType::Number:
            tokenStr += "number, " + QString::fromStdString(token.value);
            break;
        case TokenType::String:
            tokenStr += "string, \"" + QString::fromStdString(token.value) + "\"";
            break;
        case TokenType::Operator:
            tokenStr += "operator, " + QString::fromStdString(token.value);
            break;
        case TokenType::Delimiter:
            tokenStr += QString::fromStdString(token.value);
            break;
        case TokenType::Error:
            errCount++;
            errorStr = QString::number(errCount) +". Lexical Error:  Invalid token, " + QString::fromStdString(token.value) + " at line " +  QString::number(token.line) + ", column " + QString::number(token.column);
            m_errors.append(errorStr);
            break;
        default:
            tokenStr += "unknown";
        }

        if (token.type == TokenType::Error) continue;

        tokenStr += "> at line " + QString::number(token.line) +
                    ", column " + QString::number(token.column);
        m_tokens.append(tokenStr);
    }

    emit tokensChanged();
    emit errorsChanged();

    m_symbolTable.clear();
    const auto &symbols = m_lexer->getSymbolTable();
    for (const auto &entry : symbols) {
        m_symbolTable.append( QString::number(entry.id) +
                             "," + QString::fromStdString(entry.name) +
                             "," + QString::fromStdString(entry.dataType)+
                             "," + QString::fromStdString(entry.value));
    }

    emit symbolTableChanged();

    //runParser();

}

void Controller::runParser()
{
    if (!m_lexer) {
        m_errors = {"Lexer has not been run."};
        emit errorsChanged();
        return;
    }

    // Get tokens and symbol table from lexer
    const std::vector<Token>& tokenList = lexer_tokens;
    ParserSymbolTable symTab;

    // Initialize parser
    m_parser = std::make_unique<Parser>(tokenList, symTab);

    // Run parser
    std::unique_ptr<ProgramNode> ast = m_parser->parseProgram();

    if (ast) {
        //QJsonObject rootJson = astNodeToJson(ast.get());
        //QJsonDocument doc(rootJson);
        //m_parseTreeJson = doc.toJson(QJsonDocument::Compact);
        m_parseTreeJson = QString::fromStdString(ast->toParseTreeString());
    } else {
        m_parseTreeJson = "{}";
    }

    //qDebug().noquote() << m_parseTreeJson; // Pretty-printed JSON

    emit parseTreeJsonChanged();

    // Format and emit errors
    m_parserErrors.clear();
    for (const auto& error : m_parser->getErrors()) {
        QString errStr = QString("Line %1, Col %2: %3")
        .arg(error.line)
            .arg(error.col)
            .arg(QString::fromStdString(error.message));
        m_parserErrors.append(errStr);
        //qDebug() << errStr;
    }
    emit parserErrorsChanged();

    // Format and emit symbol table
    m_parserSymbolTable.clear();
    for (const auto& entry : m_parser->getSymbolTable().getEntries()) {
        QString entryStr = QString("ID: %1 ,%2,%3,%4,%5,%6")
        .arg(entry.id)
            .arg(QString::fromStdString(entry.name))
            .arg(QString::fromStdString(entry.dataType))
            .arg(QString::fromStdString(entry.value))
            .arg(QString::fromStdString(entry.role))
            .arg(QString::fromStdString(entry.scope));
        m_parserSymbolTable.append(entryStr);
        //qDebug() << entryStr;
    }
    emit parserSymbolTableChanged();

    // (Optional) TODO: Convert AST to JSON and emit a parseTreeChanged() signal for QML to draw it
}

QString Controller::code() const { return m_code; }
QStringList Controller::tokens() const { return m_tokens; }
QStringList Controller::symbolTable() const { return m_symbolTable; }

QStringList Controller::errors() const
{
    return m_errors;
}

QStringList Controller::parserErrors() const
{
    return m_parserErrors;
}

QStringList Controller::parserSymbolTable() const
{
    return m_parserSymbolTable;
}

QString Controller::parseTreeJson() const
{
    return m_parseTreeJson;
}


QJsonObject astNodeToJson(const ASTNode* node) {
    if (!node) return QJsonObject();

    QJsonObject obj;
    obj["type"] = QString::fromStdString(node->getNodeType());

    // Only include value for certain node types
    std::string nodeType = node->getNodeType();
    if (nodeType != "Program" &&
        nodeType != "Assign" &&
        nodeType != "Call") {
        obj["value"] = QString::fromStdString(node->token.value);
    }

    QJsonArray children;
    for (const auto& child : node->children) {
        children.append(astNodeToJson(child.get()));
    }

    if (!children.isEmpty()) {
        obj["children"] = children;
    }

    return obj;
}
