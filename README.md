# ComPy
# 🐍 Python Compiler Frontend (Lexer + Parser)

This project is a Python compiler frontend written in C++. It includes a **Lexical Analyzer (Lexer)**, a **Recursive Descent Parser**, and a **Graphical User Interface (GUI)** built with **Qt6/QML**. The project supports a simplified subset of the Python programming language and is designed for educational purposes, showcasing compiler design concepts such as tokenization, grammar parsing, and syntax tree construction.

---


---

## 🚀 Features

- **Lexer**:
  - Tokenizes Python source code
  - Handles keywords, identifiers, literals, operators, and indentation
  - Produces INDENT tokens for block structures like `if`, `def`, `while`, etc.

- **Parser**:
  - Recursive descent implementation
  - Parses the token stream and builds an abstract syntax tree (AST)
  - Supports basic Python syntax including functions, conditionals, and loops

- **GUI**:
  - Allows file selection and displays source code
  - Visualizes token stream and symbol table
  - Displays syntax analysis results (parse tree)

---

## 🛠️ Getting Started

### Prerequisites

- C++17 or later
- Qt 6 (with QML and Quick modules)
- CMake 3.16+


