/****************************************************************************
** Meta object code from reading C++ file 'sgsettingshandler.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgsettingshandler.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgsettingshandler.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGDblParamEdit_t {
    QByteArrayData data[5];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGDblParamEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGDblParamEdit_t qt_meta_stringdata_SGDblParamEdit = {
    {
QT_MOC_LITERAL(0, 0, 14), // "SGDblParamEdit"
QT_MOC_LITERAL(1, 15, 11), // "changeParam"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "text"
QT_MOC_LITERAL(4, 33, 10) // "resetParam"

    },
    "SGDblParamEdit\0changeParam\0\0text\0"
    "resetParam"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGDblParamEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       4,    0,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

       0        // eod
};

void SGDblParamEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGDblParamEdit *_t = static_cast<SGDblParamEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeParam((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->resetParam(); break;
        default: ;
        }
    }
}

const QMetaObject SGDblParamEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_SGDblParamEdit.data,
      qt_meta_data_SGDblParamEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGDblParamEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGDblParamEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGDblParamEdit.stringdata0))
        return static_cast<void*>(this);
    return QLineEdit::qt_metacast(_clname);
}

int SGDblParamEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_SGIntParamEdit_t {
    QByteArrayData data[5];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGIntParamEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGIntParamEdit_t qt_meta_stringdata_SGIntParamEdit = {
    {
QT_MOC_LITERAL(0, 0, 14), // "SGIntParamEdit"
QT_MOC_LITERAL(1, 15, 11), // "changeParam"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "text"
QT_MOC_LITERAL(4, 33, 10) // "resetParam"

    },
    "SGIntParamEdit\0changeParam\0\0text\0"
    "resetParam"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGIntParamEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       4,    0,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

       0        // eod
};

void SGIntParamEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGIntParamEdit *_t = static_cast<SGIntParamEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeParam((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->resetParam(); break;
        default: ;
        }
    }
}

const QMetaObject SGIntParamEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_SGIntParamEdit.data,
      qt_meta_data_SGIntParamEdit,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGIntParamEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGIntParamEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGIntParamEdit.stringdata0))
        return static_cast<void*>(this);
    return QLineEdit::qt_metacast(_clname);
}

int SGIntParamEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_SGBoolParamBox_t {
    QByteArrayData data[4];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGBoolParamBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGBoolParamBox_t qt_meta_stringdata_SGBoolParamBox = {
    {
QT_MOC_LITERAL(0, 0, 14), // "SGBoolParamBox"
QT_MOC_LITERAL(1, 15, 11), // "changeParam"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 10) // "resetParam"

    },
    "SGBoolParamBox\0changeParam\0\0resetParam"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGBoolParamBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SGBoolParamBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGBoolParamBox *_t = static_cast<SGBoolParamBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeParam(); break;
        case 1: _t->resetParam(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SGBoolParamBox::staticMetaObject = {
    { &QCheckBox::staticMetaObject, qt_meta_stringdata_SGBoolParamBox.data,
      qt_meta_data_SGBoolParamBox,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGBoolParamBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGBoolParamBox::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGBoolParamBox.stringdata0))
        return static_cast<void*>(this);
    return QCheckBox::qt_metacast(_clname);
}

int SGBoolParamBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCheckBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_SGSettingsHandler_t {
    QByteArrayData data[6];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGSettingsHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGSettingsHandler_t qt_meta_stringdata_SGSettingsHandler = {
    {
QT_MOC_LITERAL(0, 0, 17), // "SGSettingsHandler"
QT_MOC_LITERAL(1, 18, 20), // "closeSettingsHandler"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 20), // "restoreDefaultSignal"
QT_MOC_LITERAL(4, 61, 11), // "closeWindow"
QT_MOC_LITERAL(5, 73, 15) // "restoreDefaults"

    },
    "SGSettingsHandler\0closeSettingsHandler\0"
    "\0restoreDefaultSignal\0closeWindow\0"
    "restoreDefaults"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGSettingsHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   36,    2, 0x08 /* Private */,
       5,    0,   37,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SGSettingsHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGSettingsHandler *_t = static_cast<SGSettingsHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->closeSettingsHandler(); break;
        case 1: _t->restoreDefaultSignal(); break;
        case 2: _t->closeWindow(); break;
        case 3: _t->restoreDefaults(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SGSettingsHandler::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGSettingsHandler::closeSettingsHandler)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SGSettingsHandler::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGSettingsHandler::restoreDefaultSignal)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SGSettingsHandler::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SGSettingsHandler.data,
      qt_meta_data_SGSettingsHandler,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGSettingsHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGSettingsHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGSettingsHandler.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SGSettingsHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SGSettingsHandler::closeSettingsHandler()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SGSettingsHandler::restoreDefaultSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
