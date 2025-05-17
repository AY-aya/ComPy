#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <QObject>
#include <iostream>
using namespace std;


// Entry in the symbol table for identifiers
struct SymbolTableEntry {
    int id;
    string name;
    string dataType;
    string value;
};

class SymbolTable : public QObject
{
    Q_OBJECT
public:
    explicit SymbolTable(QObject *parent = nullptr);

    // Inserts a new identifier or returns its existing ID
    int insert(const string& name, const string& dataType = "unknown", const string& value = "unknown");

    // Prints the entire symbol table
    void print() const;

    void updateType(const string& name, const string& type, const string& value = "unknown");

    std::vector<SymbolTableEntry> getSymbolTable();

private:
    std::unordered_map<std::string, int> nameToId;
    std::vector<SymbolTableEntry> entries;
    int nextId = 0;


signals:
};

#endif // SYMBOLTABLE_H
