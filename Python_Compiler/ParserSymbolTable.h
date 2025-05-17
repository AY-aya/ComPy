#ifndef PARSERSYMBOLTABLE_H
#define PARSERSYMBOLTABLE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct ParserSymbolTableEntry {
    int id;
    string name;
    string dataType;
    string value;
    string role;
    string scope;  // Added to track scope information
};

class ParserSymbolTable {
private:
    unordered_map<string, vector<int>> nameToIds;
    vector<ParserSymbolTableEntry> entries;
    vector<unordered_map<string, int>> scopes;
    vector<string> scopeNames;
    int nextId;

public:
    ParserSymbolTable() : nextId(0) {
        scopes.emplace_back(); // global scope
        scopeNames.push_back("global");
    }

    vector<ParserSymbolTableEntry> getEntries(){
        return entries;
    }

    void beginScope(const string& scopeName = "") {
        scopes.emplace_back();
        scopeNames.push_back(scopeName);
    }

    void endScope() {
        if (scopes.size() > 1) {
            scopes.pop_back();
            scopeNames.pop_back();
        }
    }

    string getCurrentScope() const {
        return scopeNames.empty() ? "global" : scopeNames.back();
    }

    void declare(const string& name, const string& type, const string& role, const string& value = "unknown") {
        if (scopes.empty()) {
            scopes.emplace_back();
            scopeNames.push_back("global");
        }
        auto& currentScope = scopes.back();
        string currentScopeName = scopeNames.back();

        // Check if already declared in current scope
        auto it = currentScope.find(name);
        if (it != currentScope.end()) {
            int id = it->second;
            if (entries[id].dataType == "unknown") {
                entries[id].dataType = type;
            }
            entries[id].role = role;
            if (value != "unknown") {
                entries[id].value = value;
            }
            return;
        }

        // Check if exists in any scope
        auto itAll = nameToIds.find(name);
        int id;
        if (itAll == nameToIds.end()) {
            id = nextId++;
            ParserSymbolTableEntry entry = {id, name, type, value, role, currentScopeName};
            entries.push_back(entry);
            nameToIds[name] = vector<int>{id};
        } else {
            id = nextId++;
            ParserSymbolTableEntry entry = {id, name, type, value, role, currentScopeName};
            entries.push_back(entry);
            itAll->second.push_back(id);
        }
        currentScope[name] = id;
    }

    void updateValue(const string& name, const string& value) {
        for (int i = (int)scopes.size() - 1; i >= 0; --i) {
            auto& scope = scopes[i];
            auto sit = scope.find(name);
            if (sit != scope.end()) {
                int id = sit->second;
                entries[id].value = value;
                return;
            }
        }
    }

    void updateType(const string& name, const string& type) {
        for (int i = (int)scopes.size() - 1; i >= 0; --i) {
            auto& scope = scopes[i];
            auto sit = scope.find(name);
            if (sit != scope.end()) {
                int id = sit->second;
                entries[id].dataType = type;
                return;
            }
        }
    }

    void print() const {
        cout << "\nSymbol Table:\n";
        cout << "Scope hierarchy: ";
        for (const auto& scope : scopeNames) {
            cout << scope << " > ";
        }
        cout << "\n\n";

        for (const auto& entry : entries) {
            cout << "ID: " << entry.id
                 << ", Name: " << entry.name
                 << ", Type: " << entry.dataType
                 << ", Value: " << entry.value
                 << ", Role: " << entry.role
                 << ", Scope: " << entry.scope
                 << '\n';
        }
    }

    int lookup(const string& name) const {
        for (int i = (int)scopes.size() - 1; i >= 0; --i) {
            const auto& scope = scopes[i];
            auto it = scope.find(name);
            if (it != scope.end()) {
                return it->second;
            }
        }
        return -1;
    }

    ParserSymbolTableEntry* lookupEntry(const string& name) {
        for (int i = (int)scopes.size() - 1; i >= 0; --i) {
            const auto& scope = scopes[i];
            auto it = scope.find(name);
            if (it != scope.end()) {
                int id = it->second;
                if (id >= 0 && id < (int)entries.size()) {
                    return &entries[id];
                }
            }
        }
        return nullptr;
    }

    // Also add a const version for const contexts
    const ParserSymbolTableEntry* lookupEntry(const string& name) const {
        for (int i = (int)scopes.size() - 1; i >= 0; --i) {
            const auto& scope = scopes[i];
            auto it = scope.find(name);
            if (it != scope.end()) {
                int id = it->second;
                if (id >= 0 && id < (int)entries.size()) {
                    return &entries[id];
                }
            }
        }
        return nullptr;
    }
};

#endif // PARSERSYMBOLTABLE_H
