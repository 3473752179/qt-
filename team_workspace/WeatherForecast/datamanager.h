#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include "citymodel.h"
#include "weathermodel.h"
#include "forecastmodel.h"
#include "enums.h"

// ==================== 成员2：数据管理模块 ====================
// 负责数据持久化、缓存管理、配置存储

class DataManager : public QObject
{
    Q_OBJECT
    
public:
    // --- 单例模式 ---
    static DataManager& instance();
    
    // --- 收藏城市管理 ---
    QList<CityModel*> loadFavoriteCities();
    void saveFavoriteCities(const QList<CityModel*>& cities);
    void addFavoriteCity(CityModel* city);
    void removeFavoriteCity(const QString& cityId);
    bool isFavoriteCity(const QString& cityId);
    
    // --- 默认城市管理 ---
    CityModel* loadDefaultCity();
    void saveDefaultCity(const CityModel& city);
    
    // --- 主题设置 ---
    ThemeMode loadThemeMode();
    void saveThemeMode(ThemeMode mode);
    
    // --- 天气数据缓存 ---
    void cacheWeatherData(const QString& cityId, WeatherModel* weather);
    WeatherModel* loadCachedWeather(const QString& cityId);
    
    void cacheForecastData(const QString& cityId, ForecastModel* forecast);
    ForecastModel* loadCachedForecast(const QString& cityId);
    
    // --- 缓存清理 ---
    void clearCache();
    
private:
    // --- 构造函数 ---
    DataManager(QObject* parent = nullptr);
    ~DataManager();
    
    // --- 文件路径获取 ---
    QString getAppDataPath();
    QString getCitiesFilePath();
    QString getConfigFilePath();
    QString getWeatherCachePath(const QString& cityId);
    QString getForecastCachePath(const QString& cityId);
    
    // --- 成员变量 ---
    QList<CityModel*> m_favoriteCities;
};

#endif // DATAMANAGER_H