/****************************************************************************
** Meta object code from reading C++ file 'Controller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../Controller.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSControllerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSControllerENDCLASS = QtMocHelpers::stringData(
    "Controller",
    "codeChanged",
    "",
    "tokensChanged",
    "symbolTableChanged",
    "errorsChanged",
    "parserErrorsChanged",
    "parserSymbolTableChanged",
    "parseTreeJsonChanged",
    "loadFile",
    "path",
    "clearCode",
    "runLexer",
    "runParser",
    "code",
    "tokens",
    "symbolTable",
    "errors",
    "parserSymbolTable",
    "parserErrors",
    "parseTreeJson"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSControllerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       7,   93, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    8 /* Public */,
       3,    0,   81,    2, 0x06,    9 /* Public */,
       4,    0,   82,    2, 0x06,   10 /* Public */,
       5,    0,   83,    2, 0x06,   11 /* Public */,
       6,    0,   84,    2, 0x06,   12 /* Public */,
       7,    0,   85,    2, 0x06,   13 /* Public */,
       8,    0,   86,    2, 0x06,   14 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       9,    1,   87,    2, 0x02,   15 /* Public */,
      11,    0,   90,    2, 0x02,   17 /* Public */,
      12,    0,   91,    2, 0x02,   18 /* Public */,
      13,    0,   92,    2, 0x02,   19 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      14, QMetaType::QString, 0x00015001, uint(0), 0,
      15, QMetaType::QStringList, 0x00015001, uint(1), 0,
      16, QMetaType::QStringList, 0x00015001, uint(2), 0,
      17, QMetaType::QStringList, 0x00015001, uint(3), 0,
      18, QMetaType::QStringList, 0x00015001, uint(5), 0,
      19, QMetaType::QStringList, 0x00015001, uint(4), 0,
      20, QMetaType::QString, 0x00015001, uint(6), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject Controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSControllerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSControllerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSControllerENDCLASS_t,
        // property 'code'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'tokens'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'symbolTable'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'errors'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'parserSymbolTable'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'parserErrors'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'parseTreeJson'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Controller, std::true_type>,
        // method 'codeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'tokensChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'symbolTableChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'errorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'parserErrorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'parserSymbolTableChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'parseTreeJsonChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'clearCode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'runLexer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'runParser'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Controller *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->codeChanged(); break;
        case 1: _t->tokensChanged(); break;
        case 2: _t->symbolTableChanged(); break;
        case 3: _t->errorsChanged(); break;
        case 4: _t->parserErrorsChanged(); break;
        case 5: _t->parserSymbolTableChanged(); break;
        case 6: _t->parseTreeJsonChanged(); break;
        case 7: _t->loadFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->clearCode(); break;
        case 9: _t->runLexer(); break;
        case 10: _t->runParser(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::codeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::tokensChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::symbolTableChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::errorsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::parserErrorsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::parserSymbolTableChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Controller::*)();
            if (_t _q_method = &Controller::parseTreeJsonChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Controller *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->code(); break;
        case 1: *reinterpret_cast< QStringList*>(_v) = _t->tokens(); break;
        case 2: *reinterpret_cast< QStringList*>(_v) = _t->symbolTable(); break;
        case 3: *reinterpret_cast< QStringList*>(_v) = _t->errors(); break;
        case 4: *reinterpret_cast< QStringList*>(_v) = _t->parserSymbolTable(); break;
        case 5: *reinterpret_cast< QStringList*>(_v) = _t->parserErrors(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->parseTreeJson(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSControllerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Controller::codeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Controller::tokensChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Controller::symbolTableChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Controller::errorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Controller::parserErrorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Controller::parserSymbolTableChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Controller::parseTreeJsonChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
