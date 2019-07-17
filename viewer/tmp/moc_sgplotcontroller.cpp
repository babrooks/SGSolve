/****************************************************************************
** Meta object code from reading C++ file 'sgplotcontroller.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgplotcontroller.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgplotcontroller.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGPlotController_t {
    QByteArrayData data[16];
    char stringdata0[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGPlotController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGPlotController_t qt_meta_stringdata_SGPlotController = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SGPlotController"
QT_MOC_LITERAL(1, 17, 15), // "solutionChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 13), // "actionChanged"
QT_MOC_LITERAL(4, 48, 16), // "iterationChanged"
QT_MOC_LITERAL(5, 65, 10), // "changeMode"
QT_MOC_LITERAL(6, 76, 7), // "newMode"
QT_MOC_LITERAL(7, 84, 16), // "iterSliderUpdate"
QT_MOC_LITERAL(8, 101, 5), // "value"
QT_MOC_LITERAL(9, 107, 15), // "setSliderRanges"
QT_MOC_LITERAL(10, 123, 5), // "start"
QT_MOC_LITERAL(11, 129, 3), // "end"
QT_MOC_LITERAL(12, 133, 10), // "prevAction"
QT_MOC_LITERAL(13, 144, 10), // "nextAction"
QT_MOC_LITERAL(14, 155, 12), // "changeAction"
QT_MOC_LITERAL(15, 168, 9) // "newAction"

    },
    "SGPlotController\0solutionChanged\0\0"
    "actionChanged\0iterationChanged\0"
    "changeMode\0newMode\0iterSliderUpdate\0"
    "value\0setSliderRanges\0start\0end\0"
    "prevAction\0nextAction\0changeAction\0"
    "newAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGPlotController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   62,    2, 0x0a /* Public */,
       7,    1,   65,    2, 0x0a /* Public */,
       9,    2,   68,    2, 0x0a /* Public */,
      12,    0,   73,    2, 0x0a /* Public */,
      13,    0,   74,    2, 0x0a /* Public */,
      14,    1,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,

       0        // eod
};

void SGPlotController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGPlotController *_t = static_cast<SGPlotController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->solutionChanged(); break;
        case 1: _t->actionChanged(); break;
        case 2: _t->iterationChanged(); break;
        case 3: _t->changeMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->iterSliderUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setSliderRanges((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->prevAction(); break;
        case 7: _t->nextAction(); break;
        case 8: _t->changeAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SGPlotController::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGPlotController::solutionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SGPlotController::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGPlotController::actionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SGPlotController::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGPlotController::iterationChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject SGPlotController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SGPlotController.data,
      qt_meta_data_SGPlotController,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGPlotController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGPlotController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGPlotController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SGPlotController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SGPlotController::solutionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SGPlotController::actionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SGPlotController::iterationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
