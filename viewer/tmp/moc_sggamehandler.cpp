/****************************************************************************
** Meta object code from reading C++ file 'sggamehandler.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sggamehandler.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sggamehandler.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGGameHandler_t {
    QByteArrayData data[20];
    char stringdata0[240];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGGameHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGGameHandler_t qt_meta_stringdata_SGGameHandler = {
    {
QT_MOC_LITERAL(0, 0, 13), // "SGGameHandler"
QT_MOC_LITERAL(1, 14, 19), // "currentStateChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 4), // "newS"
QT_MOC_LITERAL(4, 40, 10), // "stateAdded"
QT_MOC_LITERAL(5, 51, 12), // "action1Added"
QT_MOC_LITERAL(6, 64, 12), // "action2Added"
QT_MOC_LITERAL(7, 77, 11), // "actionAdded"
QT_MOC_LITERAL(8, 89, 6), // "player"
QT_MOC_LITERAL(9, 96, 12), // "stateRemoved"
QT_MOC_LITERAL(10, 109, 14), // "action1Removed"
QT_MOC_LITERAL(11, 124, 14), // "action2Removed"
QT_MOC_LITERAL(12, 139, 13), // "actionRemoved"
QT_MOC_LITERAL(13, 153, 21), // "discountFactorChanged"
QT_MOC_LITERAL(14, 175, 4), // "text"
QT_MOC_LITERAL(15, 180, 15), // "errorTolChanged"
QT_MOC_LITERAL(16, 196, 9), // "nextState"
QT_MOC_LITERAL(17, 206, 9), // "prevState"
QT_MOC_LITERAL(18, 216, 14), // "setConstrained"
QT_MOC_LITERAL(19, 231, 8) // "newState"

    },
    "SGGameHandler\0currentStateChanged\0\0"
    "newS\0stateAdded\0action1Added\0action2Added\0"
    "actionAdded\0player\0stateRemoved\0"
    "action1Removed\0action2Removed\0"
    "actionRemoved\0discountFactorChanged\0"
    "text\0errorTolChanged\0nextState\0prevState\0"
    "setConstrained\0newState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGGameHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x08 /* Private */,
       4,    0,   87,    2, 0x08 /* Private */,
       5,    0,   88,    2, 0x08 /* Private */,
       6,    0,   89,    2, 0x08 /* Private */,
       7,    1,   90,    2, 0x08 /* Private */,
       9,    0,   93,    2, 0x08 /* Private */,
      10,    0,   94,    2, 0x08 /* Private */,
      11,    0,   95,    2, 0x08 /* Private */,
      12,    1,   96,    2, 0x08 /* Private */,
      13,    1,   99,    2, 0x08 /* Private */,
      15,    1,  102,    2, 0x08 /* Private */,
      16,    0,  105,    2, 0x08 /* Private */,
      17,    0,  106,    2, 0x08 /* Private */,
      18,    1,  107,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,

       0        // eod
};

void SGGameHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGGameHandler *_t = static_cast<SGGameHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->stateAdded(); break;
        case 2: _t->action1Added(); break;
        case 3: _t->action2Added(); break;
        case 4: _t->actionAdded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->stateRemoved(); break;
        case 6: _t->action1Removed(); break;
        case 7: _t->action2Removed(); break;
        case 8: _t->actionRemoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->discountFactorChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->errorTolChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->nextState(); break;
        case 12: _t->prevState(); break;
        case 13: _t->setConstrained((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject SGGameHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SGGameHandler.data,
      qt_meta_data_SGGameHandler,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGGameHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGGameHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGGameHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SGGameHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
