#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

// ==================== 成员2：数据管理模块 ====================
// 负责数据持久化、缓存管理、配置存储

// ==================== 构造与初始化 ====================

DataManager::DataManager(QObject* parent)
    : QObject(parent)
{
}

DataManager::~DataManager()
{
    qDeleteAll(m_favoriteCities);
}

DataManager& DataManager::instance()
{
    static DataManager instance;
    return instance;
}

// ==================== 文件路径管理 ====================

QString DataManager::getAppDataPath()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }
    return path;
}

QString DataManager::getCitiesFilePath()
{
    return QDir(getAppDataPath()).filePath("cities.json");
}

QString DataManager::getConfigFilePath()
{
    return QDir(getAppDataPath()).filePath("config.json");
}

QString DataManager::getWeatherCachePath(const QString& cityId)
{
    return QDir(getAppDataPath()).filePath(QString("weather_%1.json").arg(cityId));
}

QString DataManager::getForecastCachePath(const QString& cityId)
{
    return QDir(getAppDataPath()).filePath(QString("forecast_%1.json").arg(cityId));
}

// ==================== 收藏城市管理 ====================

QList<CityModel*> DataManager::loadFavoriteCities()
{
    QFile file(getCitiesFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return m_favoriteCities;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isArray()) {
        return m_favoriteCities;
    }
    
    qDeleteAll(m_favoriteCities);
    m_favoriteCities.clear();
    
    QJsonArray citiesArray = doc.array();
    for (int i = 0; i < citiesArray.size(); ++i) {
        QJsonObject obj = citiesArray[i].toObject();
        CityModel* city = new CityModel();
        city->setId(obj["id"].toString());
        city->setName(obj["name"].toString());
        city->setCountry(obj["country"].toString());
        city->setLatitude(obj["latitude"].toDouble());
        city->setLongitude(obj["longitude"].toDouble());
        m_favoriteCities.append(city);
    }
    
    return m_favoriteCities;
}

void DataManager::saveFavoriteCities(const QList<CityModel*>& cities)
{
    QJsonArray citiesArray;
    for (CityModel* city : cities) {
        QJsonObject obj;
        obj["id"] = city->id();
        obj["name"] = city->name();
        obj["country"] = city->country();
        obj["latitude"] = city->latitude();
        obj["longitude"] = city->longitude();
        citiesArray.append(obj);
    }
    
    QJsonDocument doc(citiesArray);
    QFile file(getCitiesFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
    
    m_favoriteCities = cities;
}

void DataManager::addFavoriteCity(CityModel* city)
{
    if (!city || isFavoriteCity(city->id())) return;
    
    m_favoriteCities.append(new CityModel(*city));
    saveFavoriteCities(m_favoriteCities);
}

void DataManager::removeFavoriteCity(const QString& cityId)
{
    for (int i = 0; i < m_favoriteCities.size(); ++i) {
        if (m_favoriteCities[i]->id() == cityId) {
            delete m_favoriteCities.takeAt(i);
            saveFavoriteCities(m_favoriteCities);
            return;
        }
    }
}

bool DataManager::isFavoriteCity(const QString& cityId)
{
    for (CityModel* city : m_favoriteCities) {
        if (city->id() == cityId) {
            return true;
        }
    }
    return false;
}

// ==================== 默认城市管理 ====================

CityModel* DataManager::loadDefaultCity()
{
    QFile file(getConfigFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) {
        return nullptr;
    }
    
    QJsonObject obj = doc.object();
    if (!obj.contains("defaultCity")) {
        return nullptr;
    }
    
    QJsonObject cityObj = obj["defaultCity"].toObject();
    CityModel* city = new CityModel();
    city->setId(cityObj["id"].toString());
    city->setName(cityObj["name"].toString());
    city->setCountry(cityObj["country"].toString());
    city->setLatitude(cityObj["latitude"].toDouble());
    city->setLongitude(cityObj["longitude"].toDouble());
    
    return city;
}

void DataManager::saveDefaultCity(const CityModel& city)
{
    QJsonObject config;
    QJsonObject cityObj;
    cityObj["id"] = city.id();
    cityObj["name"] = city.name();
    cityObj["country"] = city.country();
    cityObj["latitude"] = city.latitude();
    cityObj["longitude"] = city.longitude();
    config["defaultCity"] = cityObj;
    
    QJsonDocument doc(config);
    QFile file(getConfigFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

// ==================== 主题设置管理 ====================

ThemeMode DataManager::loadThemeMode()
{
    QFile file(getConfigFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return ThemeMode::Light;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) {
        return ThemeMode::Light;
    }
    
    QJsonObject obj = doc.object();
    if (obj["theme"].toString() == "dark") {
        return ThemeMode::Dark;
    }
    
    return ThemeMode::Light;
}

void DataManager::saveThemeMode(ThemeMode mode)
{
    QJsonObject config;
    config["theme"] = mode == ThemeMode::Dark ? "dark" : "light";
    
    QJsonDocument doc(config);
    QFile file(getConfigFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

// ==================== 天气数据缓存 ====================

void DataManager::cacheWeatherData(const QString& cityId, WeatherModel* weather)
{
    QJsonObject obj;
    obj["cityId"] = weather->cityId();
    obj["cityName"] = weather->cityName();
    obj["temperature"] = weather->temperature();
    obj["feelsLike"] = weather->feelsLike();
    obj["humidity"] = weather->humidity();
    obj["windSpeed"] = weather->windSpeed();
    obj["pressure"] = weather->pressure();
    obj["visibility"] = weather->visibility();
    obj["updateTime"] = weather->updateTime().toString();
    
    QJsonDocument doc(obj);
    QFile file(getWeatherCachePath(cityId));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

WeatherModel* DataManager::loadCachedWeather(const QString& cityId)
{
    QFile file(getWeatherCachePath(cityId));
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) {
        return nullptr;
    }
    
    QJsonObject obj = doc.object();
    WeatherModel* weather = new WeatherModel();
    weather->setCityId(obj["cityId"].toString());
    weather->setCityName(obj["cityName"].toString());
    weather->setTemperature(obj["temperature"].toDouble());
    weather->setFeelsLike(obj["feelsLike"].toDouble());
    weather->setHumidity(obj["humidity"].toInt());
    weather->setWindSpeed(obj["windSpeed"].toInt());
    weather->setPressure(obj["pressure"].toInt());
    weather->setVisibility(obj["visibility"].toInt());
    weather->setUpdateTime(QDateTime::fromString(obj["updateTime"].toString()));
    
    return weather;
}

void DataManager::cacheForecastData(const QString& cityId, ForecastModel* forecast)
{
    QJsonObject obj;
    obj["cityId"] = forecast->cityId();
    obj["cityName"] = forecast->cityName();
    
    QJsonArray daysArray;
    for (DailyForecast* day : forecast->forecasts()) {
        QJsonObject dayObj;
        dayObj["date"] = day->date().toString();
        dayObj["maxTemp"] = day->maxTemperature();
        dayObj["minTemp"] = day->minTemperature();
        dayObj["humidity"] = day->humidity();
        daysArray.append(dayObj);
    }
    obj["forecasts"] = daysArray;
    
    QJsonDocument doc(obj);
    QFile file(getForecastCachePath(cityId));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

ForecastModel* DataManager::loadCachedForecast(const QString& cityId)
{
    QFile file(getForecastCachePath(cityId));
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) {
        return nullptr;
    }
    
    QJsonObject obj = doc.object();
    ForecastModel* forecast = new ForecastModel();
    forecast->setCityId(obj["cityId"].toString());
    forecast->setCityName(obj["cityName"].toString());
    
    QJsonArray daysArray = obj["forecasts"].toArray();
    for (int i = 0; i < daysArray.size(); ++i) {
        QJsonObject dayObj = daysArray[i].toObject();
        DailyForecast* day = new DailyForecast();
        day->setDate(QDateTime::fromString(dayObj["date"].toString()));
        day->setMaxTemperature(dayObj["maxTemp"].toDouble());
        day->setMinTemperature(dayObj["minTemp"].toDouble());
        day->setHumidity(dayObj["humidity"].toInt());
        forecast->addForecast(day);
    }
    
    return forecast;
}

// ==================== 缓存清理 ====================

void DataManager::clearCache()
{
    QDir dir(getAppDataPath());
    QStringList filters;
    filters << "weather_*.json" << "forecast_*.json";
    QStringList files = dir.entryList(filters, QDir::Files);
    for (QString file : files) {
        dir.remove(file);
    }
}