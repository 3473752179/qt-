#include "networkmanager.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QTimer>
#include <QDebug>

// ==================== 成员1：网络请求模块 ====================
// 负责API调用、数据获取、网络状态管理（纯网络层，不负责数据解析）

// ==================== 构造与初始化 ====================

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent), m_apiKey("f6b97ea6086c366cf5ddbc55e1d0cfa4"), m_networkAvailable(true)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(10000);
    
    m_networkCheckTimer = new QTimer(this);
    m_networkCheckTimer->setInterval(5000);
    m_networkCheckTimer->start();
    
    connect(m_timeoutTimer, &QTimer::timeout, this, &NetworkManager::onNetworkTimeout);
    connect(m_networkCheckTimer, &QTimer::timeout, this, &NetworkManager::checkNetworkStatus);
}

NetworkManager::~NetworkManager()
{
}

NetworkManager& NetworkManager::instance()
{
    static NetworkManager instance;
    return instance;
}

// ==================== 网络请求接口 ====================

void NetworkManager::fetchCurrentWeather(const QString& cityId, double latitude, double longitude)
{
    emit loadingStarted();
    
    QUrl url("https://api.open-meteo.com/v1/forecast");
    QUrlQuery query;
    query.addQueryItem("latitude", QString::number(latitude, 'f', 6));
    query.addQueryItem("longitude", QString::number(longitude, 'f', 6));
    query.addQueryItem("current", "temperature_2m,relative_humidity_2m,apparent_temperature,pressure_msl,wind_speed_10m,wind_direction_10m,weather_code");
    query.addQueryItem("hourly", "temperature_2m,relative_humidity_2m,apparent_temperature,pressure_msl,wind_speed_10m,wind_direction_10m");
    query.addQueryItem("timezone", "auto");
    query.addQueryItem("forecast_days", "1");
    query.addQueryItem("forecast_hours", "24");
    query.addQueryItem("wind_speed_unit", "kmh");
    
    url.setQuery(query);
    
    qDebug() << "Weather URL:" << url.toString();
    
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply, cityId]() {
        m_timeoutTimer->stop();
        onCurrentWeatherReply(reply, cityId);
    });
    
    m_timeoutTimer->start();
}

void NetworkManager::fetchCurrentAirQuality(const QString& cityId, double latitude, double longitude)
{
    QUrl url("https://air-quality-api.open-meteo.com/v1/air-quality");
    QUrlQuery query;
    query.addQueryItem("latitude", QString::number(latitude, 'f', 6));
    query.addQueryItem("longitude", QString::number(longitude, 'f', 6));
    query.addQueryItem("current", "us_aqi,pm2_5");
    query.addQueryItem("timezone", "auto");

    url.setQuery(query);

    qDebug() << "Air quality URL:" << url.toString();

    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply, cityId]() {
        onCurrentAirQualityReply(reply, cityId);
    });
}

void NetworkManager::fetchForecast(const QString& cityId)
{
    QUrl url("https://restapi.amap.com/v3/weather/weatherInfo");
    QUrlQuery query;
    query.addQueryItem("city", cityId);
    query.addQueryItem("key", m_apiKey);
    query.addQueryItem("extensions", "all");
    
    url.setQuery(query);
    
    qDebug() << "Forecast URL:" << url.toString();
    
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReply* reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply, cityId]() {
        onForecastReply(reply, cityId);
    });
}

// ==================== API配置管理 ====================

bool NetworkManager::isNetworkAvailable() const
{
    return m_networkAvailable;
}

void NetworkManager::setApiKey(const QString& apiKey)
{
    m_apiKey = apiKey;
}

QString NetworkManager::apiKey() const
{
    return m_apiKey;
}

// ==================== 响应处理槽函数 ====================

void NetworkManager::onCurrentWeatherReply(QNetworkReply* reply, const QString& cityId)
{
    emit loadingFinished();
    
    if (reply->error() != QNetworkReply::NoError) {
        emit networkError(reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray data = reply->readAll();
    qDebug() << "Weather API response:" << QString(data);
    
    emit currentWeatherDataReceived(data, cityId);
    
    reply->deleteLater();
}

void NetworkManager::onForecastReply(QNetworkReply* reply, const QString& cityId)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit networkError(reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray data = reply->readAll();
    qDebug() << "Forecast API response:" << QString(data);
    
    emit forecastDataReceived(data, cityId);
    
    reply->deleteLater();
}

void NetworkManager::onCurrentAirQualityReply(QNetworkReply* reply, const QString& cityId)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit networkError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << "Air quality API response:" << QString(data);

    emit currentAirQualityDataReceived(data, cityId);

    reply->deleteLater();
}

void NetworkManager::onNetworkTimeout()
{
    emit networkError("Network timeout");
    emit loadingFinished();
}

void NetworkManager::checkNetworkStatus()
{
    bool previousStatus = m_networkAvailable;
    m_networkAvailable = true;
    
    if (previousStatus != m_networkAvailable) {
        emit networkStatusChanged(m_networkAvailable);
    }
}
