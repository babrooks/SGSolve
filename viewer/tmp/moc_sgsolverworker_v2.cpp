/****************************************************************************
** Meta object code from reading C++ file 'sgsolverworker_v2.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgsolverworker_v2.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgsolverworker_v2.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGSolverWorker_V2_t {
    QByteArrayData data[7];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGSolverWorker_V2_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGSolverWorker_V2_t qt_meta_stringdata_SGSolverWorker_V2 = {
    {
QT_MOC_LITERAL(0, 0, 17), // "SGSolverWorker_V2"
QT_MOC_LITERAL(1, 18, 11), // "resultReady"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 15), // "exceptionCaught"
QT_MOC_LITERAL(4, 47, 7), // "iterate"
QT_MOC_LITERAL(5, 55, 11), // "getSolution"
QT_MOC_LITERAL(6, 67, 9) // "getSolver"

    },
    "SGSolverWorker_V2\0resultReady\0\0"
    "exceptionCaught\0iterate\0getSolution\0"
    "getSolver"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGSolverWorker_V2[] = {

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
       1,    1,   39,    2, 0x06 /* Public */,
       3,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   43,    2, 0x0a /* Public */,
       5,    0,   44,    2, 0x0a /* Public */,
       6,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SGSolverWorker_V2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGSolverWorker_V2 *_t = static_cast<SGSolverWorker_V2 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resultReady((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->exceptionCaught(); break;
        case 2: _t->iterate(); break;
        case 3: _t->getSolution(); break;
        case 4: _t->getSolver(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SGSolverWorker_V2::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGSolverWorker_V2::resultReady)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SGSolverWorker_V2::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGSolverWorker_V2::exceptionCaught)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject SGSolverWorker_V2::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SGSolverWorker_V2.data,
      qt_meta_data_SGSolverWorker_V2,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGSolverWorker_V2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGSolverWorker_V2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGSolverWorker_V2.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SGSolverWorker_V2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SGSolverWorker_V2::resultReady(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SGSolverWorker_V2::exceptionCaught()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
