/****************************************************************************
** Meta object code from reading C++ file 'sgsolutionhandler_v2.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgsolutionhandler_v2.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgsolutionhandler_v2.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGSolutionHandler_V2_t {
    QByteArrayData data[11];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGSolutionHandler_V2_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGSolutionHandler_V2_t qt_meta_stringdata_SGSolutionHandler_V2 = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SGSolutionHandler_V2"
QT_MOC_LITERAL(1, 21, 16), // "nextActionSignal"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 16), // "prevActionSignal"
QT_MOC_LITERAL(4, 56, 12), // "inspectPoint"
QT_MOC_LITERAL(5, 69, 7), // "SGPoint"
QT_MOC_LITERAL(6, 77, 5), // "point"
QT_MOC_LITERAL(7, 83, 5), // "state"
QT_MOC_LITERAL(8, 89, 12), // "isDetailPlot"
QT_MOC_LITERAL(9, 102, 19), // "simulateEquilibrium"
QT_MOC_LITERAL(10, 122, 10) // "replotSlot"

    },
    "SGSolutionHandler_V2\0nextActionSignal\0"
    "\0prevActionSignal\0inspectPoint\0SGPoint\0"
    "point\0state\0isDetailPlot\0simulateEquilibrium\0"
    "replotSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGSolutionHandler_V2[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    3,   41,    2, 0x0a /* Public */,
       9,    3,   48,    2, 0x0a /* Public */,
      10,    0,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int, QMetaType::Bool,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int, QMetaType::Bool,    6,    7,    8,
    QMetaType::Void,

       0        // eod
};

void SGSolutionHandler_V2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGSolutionHandler_V2 *_t = static_cast<SGSolutionHandler_V2 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nextActionSignal(); break;
        case 1: _t->prevActionSignal(); break;
        case 2: _t->inspectPoint((*reinterpret_cast< SGPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->simulateEquilibrium((*reinterpret_cast< SGPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 4: _t->replotSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SGSolutionHandler_V2::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGSolutionHandler_V2::nextActionSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SGSolutionHandler_V2::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGSolutionHandler_V2::prevActionSignal)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject SGSolutionHandler_V2::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SGSolutionHandler_V2.data,
      qt_meta_data_SGSolutionHandler_V2,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGSolutionHandler_V2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGSolutionHandler_V2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGSolutionHandler_V2.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SGSolutionHandler_V2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SGSolutionHandler_V2::nextActionSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SGSolutionHandler_V2::prevActionSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
