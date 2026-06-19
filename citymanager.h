#ifndef CITYMANAGER_H
#define CITYMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QNetworkAccessManager>
#include "citymodel.h"
#include "forecastmodel.h"

// ==================== 成员2：数据管理模块 ====================
// 负责城市管理、城市搜索、当前城市状态

class CityManager : public QObject
{
    Q_OBJECT
    
public:
    // --- 单例模式 ---
    static CityManager& instance();
    
    // --- 收藏城市管理 ---
    void addFavoriteCity(CityModel* city);
    void removeFavoriteCity(const QString& cityId);
    QList<CityModel*> favoriteCities() const;
    bool isFavorite(const QString& cityId) const;
    
    // --- 当前城市管理 ---
    void setCurrentCity(CityModel* city);
    CityModel* currentCity() const;
    void switchToCity(const QString& cityId);
    
    // --- 城市搜索 ---
    void searchCity(const QString& keyword);
    
    // --- 预报预处理 ---
    void preprocessForecast(ForecastModel* forecast);
    
signals:
    // --- 状态变化信号 ---
    void favoriteCitiesChanged();
    void currentCityChanged(CityModel* city);
    void searchResultsReady(QList<CityModel*> cities);
    void forecastPreprocessed(ForecastModel* forecast);
    
private:
    // --- 构造函数 ---
    CityManager(QObject* parent = nullptr);
    ~CityManager();
    
    // --- 成员变量 ---
    QList<CityModel*> m_favoriteCities;
    CityModel* m_currentCity;
    QNetworkAccessManager* m_networkManager;
};

#endif // CITYMANAGER_H
