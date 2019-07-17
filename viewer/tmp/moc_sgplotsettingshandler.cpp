/****************************************************************************
** Meta object code from reading C++ file 'sgplotsettingshandler.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hpp/sgplotsettingshandler.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sgplotsettingshandler.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SGPlotFeatureBox_t {
    QByteArrayData data[4];
    char stringdata0[41];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGPlotFeatureBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGPlotFeatureBox_t qt_meta_stringdata_SGPlotFeatureBox = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SGPlotFeatureBox"
QT_MOC_LITERAL(1, 17, 11), // "changeParam"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 10) // "resetParam"

    },
    "SGPlotFeatureBox\0changeParam\0\0resetParam"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGPlotFeatureBox[] = {

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

void SGPlotFeatureBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGPlotFeatureBox *_t = static_cast<SGPlotFeatureBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeParam(); break;
        case 1: _t->resetParam(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SGPlotFeatureBox::staticMetaObject = {
    { &QCheckBox::staticMetaObject, qt_meta_stringdata_SGPlotFeatureBox.data,
      qt_meta_data_SGPlotFeatureBox,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGPlotFeatureBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGPlotFeatureBox::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGPlotFeatureBox.stringdata0))
        return static_cast<void*>(this);
    return QCheckBox::qt_metacast(_clname);
}

int SGPlotFeatureBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_SGPlotSettingsHandler_t {
    QByteArrayData data[6];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SGPlotSettingsHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SGPlotSettingsHandler_t qt_meta_stringdata_SGPlotSettingsHandler = {
    {
QT_MOC_LITERAL(0, 0, 21), // "SGPlotSettingsHandler"
QT_MOC_LITERAL(1, 22, 24), // "closePlotSettingsHandler"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 20), // "restoreDefaultSignal"
QT_MOC_LITERAL(4, 69, 11), // "closeWindow"
QT_MOC_LITERAL(5, 81, 15) // "restoreDefaults"

    },
    "SGPlotSettingsHandler\0closePlotSettingsHandler\0"
    "\0restoreDefaultSignal\0closeWindow\0"
    "restoreDefaults"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SGPlotSettingsHandler[] = {

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

void SGPlotSettingsHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SGPlotSettingsHandler *_t = static_cast<SGPlotSettingsHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->closePlotSettingsHandler(); break;
        case 1: _t->restoreDefaultSignal(); break;
        case 2: _t->closeWindow(); break;
        case 3: _t->restoreDefaults(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SGPlotSettingsHandler::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGPlotSettingsHandler::closePlotSettingsHandler)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SGPlotSettingsHandler::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SGPlotSettingsHandler::restoreDefaultSignal)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SGPlotSettingsHandler::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SGPlotSettingsHandler.data,
      qt_meta_data_SGPlotSettingsHandler,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SGPlotSettingsHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SGPlotSettingsHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SGPlotSettingsHandler.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SGPlotSettingsHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SGPlotSettingsHandler::closePlotSettingsHandler()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SGPlotSettingsHandler::restoreDefaultSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
