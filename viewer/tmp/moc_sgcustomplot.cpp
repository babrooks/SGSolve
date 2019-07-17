/****************************************************************************
** Meta object code from reading C++ file 'sgcustomplot.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgcustomplot.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgcustomplot.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGCustomPlot_t {
    QByteArrayData data[14];
    char stringdata0[151];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGCustomPlot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGCustomPlot_t qt_meta_stringdata_SGCustomPlot = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SGCustomPlot"
QT_MOC_LITERAL(1, 13, 12), // "inspectPoint"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 7), // "SGPoint"
QT_MOC_LITERAL(4, 35, 5), // "point"
QT_MOC_LITERAL(5, 41, 5), // "state"
QT_MOC_LITERAL(6, 47, 12), // "isDetailPlot"
QT_MOC_LITERAL(7, 60, 19), // "simulateEquilibrium"
QT_MOC_LITERAL(8, 80, 15), // "ShowContextMenu"
QT_MOC_LITERAL(9, 96, 3), // "pos"
QT_MOC_LITERAL(10, 100, 14), // "pointInspected"
QT_MOC_LITERAL(11, 115, 19), // "simulationRequested"
QT_MOC_LITERAL(12, 135, 7), // "savePDF"
QT_MOC_LITERAL(13, 143, 7) // "savePNG"

    },
    "SGCustomPlot\0inspectPoint\0\0SGPoint\0"
    "point\0state\0isDetailPlot\0simulateEquilibrium\0"
    "ShowContextMenu\0pos\0pointInspected\0"
    "simulationRequested\0savePDF\0savePNG"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGCustomPlot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   49,    2, 0x06 /* Public */,
       7,    3,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   63,    2, 0x08 /* Private */,
      10,    0,   66,    2, 0x08 /* Private */,
      11,    0,   67,    2, 0x08 /* Private */,
      12,    0,   68,    2, 0x08 /* Private */,
      13,    0,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Bool,    4,    5,    6,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Bool,    4,    5,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SGCustomPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGCustomPlot *_t = static_cast<SGCustomPlot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->inspectPoint((*reinterpret_cast< SGPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->simulateEquilibrium((*reinterpret_cast< SGPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->ShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: _t->pointInspected(); break;
        case 4: _t->simulationRequested(); break;
        case 5: _t->savePDF(); break;
        case 6: _t->savePNG(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SGCustomPlot::*_t)(SGPoint , int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGCustomPlot::inspectPoint)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SGCustomPlot::*_t)(SGPoint , int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGCustomPlot::simulateEquilibrium)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject SGCustomPlot::staticMetaObject = {
    { &QCustomPlot::staticMetaObject, qt_meta_stringdata_SGCustomPlot.data,
      qt_meta_data_SGCustomPlot,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGCustomPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGCustomPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGCustomPlot.stringdata0))
        return static_cast<void*>(this);
    return QCustomPlot::qt_metacast(_clname);
}

int SGCustomPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCustomPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void SGCustomPlot::inspectPoint(SGPoint _t1, int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SGCustomPlot::simulateEquilibrium(SGPoint _t1, int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
