#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <lexer.h>
#include "parser.h"

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString code READ code NOTIFY codeChanged)
    Q_PROPERTY(QStringList tokens READ tokens NOTIFY tokensChanged)
    Q_PROPERTY(QStringList symbolTable READ symbolTable NOTIFY symbolTableChanged)
    Q_PROPERTY(QStringList errors READ errors NOTIFY errorsChanged)
    Q_PROPERTY(QStringList parserSymbolTable READ parserSymbolTable NOTIFY parserSymbolTableChanged)
    Q_PROPERTY(QStringList parserErrors READ parserErrors NOTIFY parserErrorsChanged)
    Q_PROPERTY(QString parseTreeJson READ parseTreeJson NOTIFY parseTreeJsonChanged)

public:
    explicit Controller(QObject *parent = nullptr);
    Q_INVOKABLE void loadFile(const QString &path);
    Q_INVOKABLE void clearCode();
    Q_INVOKABLE void runLexer();
    Q_INVOKABLE void runParser();

    QString code() const;
    QStringList tokens() const;
    QStringList symbolTable() const;
    QStringList errors() const;

    QStringList parserErrors() const;

    QStringList parserSymbolTable() const;

    QString parseTreeJson() const;

signals:
    void codeChanged();
    void tokensChanged();
    void symbolTableChanged();
    void errorsChanged();

private:
    vector<Token> lexer_tokens;
    QString m_code;
    QStringList m_tokens;
    QStringList m_symbolTable;
    QStringList m_errors;
    std::unique_ptr<Lexer> m_lexer;
    std::unique_ptr<Parser> m_parser;


    QStringList m_parserErrors;

    QStringList m_parserSymbolTable;

    QString m_parseTreeJson;

signals:
    void parserErrorsChanged();
    void parserSymbolTableChanged();
    void parseTreeJsonChanged();
};

#endif // CONTROLLER_H
