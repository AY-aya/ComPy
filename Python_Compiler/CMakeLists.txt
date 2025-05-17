cmake_minimum_required(VERSION 3.16)

project(Python_Compiler VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

qt_add_executable(appPython_Compiler
    main.cpp
)

qt_add_qml_module(appPython_Compiler
    URI Python_Compiler
    VERSION 1.0
    QML_FILES main.qml
    SOURCES SymbolTable.h SymbolTable.cpp
    SOURCES lexer.h lexer.cpp
    SOURCES Controller.h Controller.cpp
    SOURCES AST_Node.h
    SOURCES parser.h parser.cpp
    SOURCES ParserSymbolTable.h
    RESOURCES
    assets/logo.png
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appPython_Compiler PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appPython_Compiler
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appPython_Compiler
    PRIVATE Qt6::Quick)

include(GNUInstallDirs)
install(TARGETS appPython_Compiler
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
