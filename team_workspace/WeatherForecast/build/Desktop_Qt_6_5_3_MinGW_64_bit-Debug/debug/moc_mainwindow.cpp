/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
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
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "onCitySelected",
    "",
    "CityModel*",
    "city",
    "onAddCityClicked",
    "onRemoveCityClicked",
    "onThemeToggleClicked",
    "onSettingsClicked",
    "onCityListItemClicked",
    "QListWidgetItem*",
    "item",
    "onWeatherDataReceived",
    "data",
    "cityId",
    "onForecastDataReceived",
    "onSearchResultsReady",
    "QList<CityModel*>",
    "cities"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[38];
    char stringdata0[11];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[5];
    char stringdata5[17];
    char stringdata6[20];
    char stringdata7[21];
    char stringdata8[18];
    char stringdata9[22];
    char stringdata10[17];
    char stringdata11[5];
    char stringdata12[22];
    char stringdata13[5];
    char stringdata14[7];
    char stringdata15[23];
    char stringdata16[21];
    char stringdata17[18];
    char stringdata18[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 14),  // "onCitySelected"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 10),  // "CityModel*"
        QT_MOC_LITERAL(38, 4),  // "city"
        QT_MOC_LITERAL(43, 16),  // "onAddCityClicked"
        QT_MOC_LITERAL(60, 19),  // "onRemoveCityClicked"
        QT_MOC_LITERAL(80, 20),  // "onThemeToggleClicked"
        QT_MOC_LITERAL(101, 17),  // "onSettingsClicked"
        QT_MOC_LITERAL(119, 21),  // "onCityListItemClicked"
        QT_MOC_LITERAL(141, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(158, 4),  // "item"
        QT_MOC_LITERAL(163, 21),  // "onWeatherDataReceived"
        QT_MOC_LITERAL(185, 4),  // "data"
        QT_MOC_LITERAL(190, 6),  // "cityId"
        QT_MOC_LITERAL(197, 22),  // "onForecastDataReceived"
        QT_MOC_LITERAL(220, 20),  // "onSearchResultsReady"
        QT_MOC_LITERAL(241, 17),  // "QList<CityModel*>"
        QT_MOC_LITERAL(259, 6)   // "cities"
    },
    "MainWindow",
    "onCitySelected",
    "",
    "CityModel*",
    "city",
    "onAddCityClicked",
    "onRemoveCityClicked",
    "onThemeToggleClicked",
    "onSettingsClicked",
    "onCityListItemClicked",
    "QListWidgetItem*",
    "item",
    "onWeatherDataReceived",
    "data",
    "cityId",
    "onForecastDataReceived",
    "onSearchResultsReady",
    "QList<CityModel*>",
    "cities"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x08,    1 /* Private */,
       5,    0,   71,    2, 0x08,    3 /* Private */,
       6,    0,   72,    2, 0x08,    4 /* Private */,
       7,    0,   73,    2, 0x08,    5 /* Private */,
       8,    0,   74,    2, 0x08,    6 /* Private */,
       9,    1,   75,    2, 0x08,    7 /* Private */,
      12,    2,   78,    2, 0x08,    9 /* Private */,
      15,    2,   83,    2, 0x08,   12 /* Private */,
      16,    1,   88,    2, 0x08,   15 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::QString,   13,   14,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::QString,   13,   14,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onCitySelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CityModel *, std::false_type>,
        // method 'onAddCityClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRemoveCityClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onThemeToggleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSettingsClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCityListItemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onWeatherDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onForecastDataReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onSearchResultsReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QList<CityModel*>, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCitySelected((*reinterpret_cast< std::add_pointer_t<CityModel*>>(_a[1]))); break;
        case 1: _t->onAddCityClicked(); break;
        case 2: _t->onRemoveCityClicked(); break;
        case 3: _t->onThemeToggleClicked(); break;
        case 4: _t->onSettingsClicked(); break;
        case 5: _t->onCityListItemClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 6: _t->onWeatherDataReceived((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->onForecastDataReceived((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->onSearchResultsReady((*reinterpret_cast< std::add_pointer_t<QList<CityModel*>>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
