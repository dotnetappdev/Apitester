/****************************************************************************
** Meta object code from reading C++ file 'RequestPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/RequestPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RequestPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RequestPanel_t {
    QByteArrayData data[15];
    char stringdata0[186];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RequestPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RequestPanel_t qt_meta_stringdata_RequestPanel = {
    {
QT_MOC_LITERAL(0, 0, 12), // "RequestPanel"
QT_MOC_LITERAL(1, 13, 16), // "responseReceived"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "response"
QT_MOC_LITERAL(4, 40, 10), // "statusCode"
QT_MOC_LITERAL(5, 51, 12), // "responseTime"
QT_MOC_LITERAL(6, 64, 7), // "headers"
QT_MOC_LITERAL(7, 72, 14), // "requestStarted"
QT_MOC_LITERAL(8, 87, 15), // "requestFinished"
QT_MOC_LITERAL(9, 103, 11), // "sendRequest"
QT_MOC_LITERAL(10, 115, 18), // "onResponseReceived"
QT_MOC_LITERAL(11, 134, 9), // "addHeader"
QT_MOC_LITERAL(12, 144, 12), // "removeHeader"
QT_MOC_LITERAL(13, 157, 12), // "addParameter"
QT_MOC_LITERAL(14, 170, 15) // "removeParameter"

    },
    "RequestPanel\0responseReceived\0\0response\0"
    "statusCode\0responseTime\0headers\0"
    "requestStarted\0requestFinished\0"
    "sendRequest\0onResponseReceived\0addHeader\0"
    "removeHeader\0addParameter\0removeParameter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RequestPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   59,    2, 0x06 /* Public */,
       7,    0,   68,    2, 0x06 /* Public */,
       8,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   70,    2, 0x08 /* Private */,
      10,    4,   71,    2, 0x08 /* Private */,
      11,    0,   80,    2, 0x08 /* Private */,
      12,    0,   81,    2, 0x08 /* Private */,
      13,    0,   82,    2, 0x08 /* Private */,
      14,    0,   83,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::LongLong, QMetaType::QString,    3,    4,    5,    6,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::LongLong, QMetaType::QString,    3,    4,    5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RequestPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RequestPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->responseReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 1: _t->requestStarted(); break;
        case 2: _t->requestFinished(); break;
        case 3: _t->sendRequest(); break;
        case 4: _t->onResponseReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 5: _t->addHeader(); break;
        case 6: _t->removeHeader(); break;
        case 7: _t->addParameter(); break;
        case 8: _t->removeParameter(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RequestPanel::*)(const QString & , int , qint64 , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RequestPanel::responseReceived)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RequestPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RequestPanel::requestStarted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RequestPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RequestPanel::requestFinished)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RequestPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RequestPanel.data,
    qt_meta_data_RequestPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RequestPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RequestPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RequestPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RequestPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void RequestPanel::responseReceived(const QString & _t1, int _t2, qint64 _t3, const QString & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RequestPanel::requestStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RequestPanel::requestFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
