#include "citymanager.h"
#include "datamanager.h"
#include "networkmanager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDebug>

// ==================== 成员2：数据管理模块 ====================
// 负责城市管理、城市搜索、当前城市状态

// ==================== 构造与初始化 ====================

CityManager::CityManager(QObject* parent)
    : QObject(parent), m_currentCity(nullptr), m_networkManager(new QNetworkAccessManager(this))
{
    m_favoriteCities = DataManager::instance().loadFavoriteCities();
}

CityManager::~CityManager()
{
    delete m_currentCity;
    qDeleteAll(m_favoriteCities);
}

CityManager& CityManager::instance()
{
    static CityManager instance;
    return instance;
}

// ==================== 收藏城市管理 ====================

void CityManager::addFavoriteCity(CityModel* city)
{
    if (!city || isFavorite(city->id())) return;
    
    m_favoriteCities.append(new CityModel(*city));
    DataManager::instance().saveFavoriteCities(m_favoriteCities);
    emit favoriteCitiesChanged();
}

void CityManager::removeFavoriteCity(const QString& cityId)
{
    for (int i = 0; i < m_favoriteCities.size(); ++i) {
        if (m_favoriteCities[i]->id() == cityId) {
            delete m_favoriteCities.takeAt(i);
            DataManager::instance().saveFavoriteCities(m_favoriteCities);
            emit favoriteCitiesChanged();
            return;
        }
    }
}

QList<CityModel*> CityManager::favoriteCities() const
{
    return m_favoriteCities;
}

bool CityManager::isFavorite(const QString& cityId) const
{
    for (CityModel* city : m_favoriteCities) {
        if (city->id() == cityId) {
            return true;
        }
    }
    return false;
}

// ==================== 当前城市管理 ====================

void CityManager::setCurrentCity(CityModel* city)
{
    delete m_currentCity;
    m_currentCity = city ? new CityModel(*city) : nullptr;
    
    if (m_currentCity) {
        DataManager::instance().saveDefaultCity(*m_currentCity);
    }
    
    emit currentCityChanged(m_currentCity);
}

CityModel* CityManager::currentCity() const
{
    return m_currentCity;
}

void CityManager::switchToCity(const QString& cityId)
{
    for (CityModel* city : m_favoriteCities) {
        if (city->id() == cityId) {
            setCurrentCity(city);
            return;
        }
    }
}

// ==================== 城市搜索 ====================

void CityManager::searchCity(const QString& keyword)
{
    QString apiKey = NetworkManager::instance().apiKey();
    
    QString url = QString("https://restapi.amap.com/v3/geocode/geo?address=%1&key=%2").arg(QUrl::toPercentEncoding(keyword), apiKey);
    
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    
    QNetworkReply* reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply, keyword]() {
        QList<CityModel*> results;
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj["status"].toString() == "1") {
                    QJsonArray geocodes = obj["geocodes"].toArray();
                    for (int i = 0; i < geocodes.size(); ++i) {
                        QJsonObject geo = geocodes[i].toObject();
                        QString adcode = geo["adcode"].toString();
                        QString formattedAddress = geo["formatted_address"].toString();
                        QString city = geo["city"].toString();
                        
                        if (city.isEmpty()) {
                            QString province = geo["province"].toString();
                            if (!province.isEmpty()) {
                                city = province;
                            } else {
                                city = formattedAddress;
                            }
                        }
                        
                        QString location = geo["location"].toString();
                        QStringList locParts = location.split(",");
                        double lon = 0.0, lat = 0.0;
                        if (locParts.size() >= 2) {
                            lon = locParts[0].toDouble();
                            lat = locParts[1].toDouble();
                        }
                        
                        CityModel* cityModel = new CityModel(adcode, city, "中国", lat, lon);
                        results.append(cityModel);
                    }
                }
            }
        }
        
        // --- 本地备用城市列表 ---
        if (results.isEmpty()) {
            QStringList cityData = {
                "110000,北京,中国,39.9042,116.4074",
                "310000,上海,中国,31.2304,121.4737",
                "440100,广州,中国,23.1291,113.2644",
                "440300,深圳,中国,22.5431,114.0579",
                "330100,杭州,中国,30.2741,120.1552",
                "320100,南京,中国,32.0603,118.7969",
                "510100,成都,中国,30.5728,104.0668",
                "500000,重庆,中国,29.4316,106.9123",
                "420100,武汉,中国,30.5928,114.3055",
                "610100,西安,中国,34.2619,108.9463",
                "450300,桂林,中国,25.2741,110.2993",
                "440600,佛山,中国,23.0226,113.1246",
                "440700,江门,中国,22.5470,113.0911",
                "330200,宁波,中国,29.8683,121.5440",
                "330300,温州,中国,28.0158,120.6996"
            };
            
            for (QString data : cityData) {
                QStringList parts = data.split(",");
                if (parts.size() >= 5) {
                    QString name = parts[1];
                    if (name.contains(keyword)) {
                        CityModel* city = new CityModel(parts[0], parts[1], parts[2],
                                                        parts[3].toDouble(), parts[4].toDouble());
                        results.append(city);
                    }
                }
            }
        }
        
        reply->deleteLater();
        emit searchResultsReady(results);
    });
}

// ==================== 预报预处理 ====================

void CityManager::preprocessForecast(ForecastModel* forecast)
{
    if (!forecast) return;
    
    emit forecastPreprocessed(forecast);
}