#include "SymbolTable.h"

SymbolTable::SymbolTable(QObject *parent)
    : QObject{parent}
{}

int SymbolTable::insert(const string &name, const string &dataType, const string &value) {
    if (nameToId.find(name) == nameToId.end()) {
        nameToId[name] = nextId;
        entries.push_back({ nextId, name, dataType, value });
        return nextId++;
    }
    return nameToId[name];
}

void SymbolTable::print() const {
    cout << "\nSymbol Table:\n";
    for (const auto& entry : entries) {
        cout << "ID: " << entry.id << ", Name: " << entry.name
             << ", Type: " << entry.dataType << ", Value: " << entry.value << '\n';
    }
}

void SymbolTable::updateType(const string &name, const string &type, const string &value) {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        int index = it->second;
        if (entries[index].dataType == "unknown")
            entries[index].dataType = type;
        if (entries[index].value == "unknown" && value != "unknown")
            entries[index].value = value;
    }
}



std::vector<SymbolTableEntry> SymbolTable::getSymbolTable()
{
    return entries;
}
