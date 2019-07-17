/****************************************************************************
** Meta object code from reading C++ file 'sgstatecombomodel_v2.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgstatecombomodel_v2.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgstatecombomodel_v2.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGStateComboModel_V2_t {
    QByteArrayData data[5];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGStateComboModel_V2_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGStateComboModel_V2_t qt_meta_stringdata_SGStateComboModel_V2 = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SGStateComboModel_V2"
QT_MOC_LITERAL(1, 21, 11), // "changeState"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 5), // "index"
QT_MOC_LITERAL(4, 40, 12) // "changeLayout"

    },
    "SGStateComboModel_V2\0changeState\0\0"
    "index\0changeLayout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGStateComboModel_V2[] = {

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
       1,    1,   24,    2, 0x0a /* Public */,
       4,    0,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

       0        // eod
};

void SGStateComboModel_V2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGStateComboModel_V2 *_t = static_cast<SGStateComboModel_V2 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->changeLayout(); break;
        default: ;
        }
    }
}

const QMetaObject SGStateComboModel_V2::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_SGStateComboModel_V2.data,
      qt_meta_data_SGStateComboModel_V2,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGStateComboModel_V2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGStateComboModel_V2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGStateComboModel_V2.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int SGStateComboModel_V2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
