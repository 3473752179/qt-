#ifndef FORECASTMODEL_H
#define FORECASTMODEL_H

#include <QString>
#include <QList>
#include <QDateTime>
#include "enums.h"

// ==================== 成员2：数据管理模块 ====================
// 负责预报数据模型定义

// ==================== 每日预报类 ====================

class DailyForecast
{
public:
    // --- 构造函数 ---
    DailyForecast();
    
    // --- 日期属性 ---
    QDateTime date() const;
    void setDate(const QDateTime& date);
    
    QString week() const;
    void setWeek(const QString& week);
    
    // --- 温度属性 ---
    double maxTemperature() const;
    void setMaxTemperature(double maxTemperature);
    
    double minTemperature() const;
    void setMinTemperature(double minTemperature);
    
    // --- 天气状况属性 ---
    int humidity() const;
    void setHumidity(int humidity);
    
    WeatherType weatherType() const;
    void setWeatherType(WeatherType weatherType);
    
    QString weatherDescription() const;
    void setWeatherDescription(const QString& weatherDescription);
    
    // --- 早晚天气 ---
    QString dayWeather() const;
    void setDayWeather(const QString& weather);
    
    QString nightWeather() const;
    void setNightWeather(const QString& weather);
    
    // --- 风向风力 ---
    QString dayWind() const;
    void setDayWind(const QString& wind);
    
    QString nightWind() const;
    void setNightWind(const QString& wind);
    
    QString dayPower() const;
    void setDayPower(const QString& power);
    
    QString nightPower() const;
    void setNightPower(const QString& power);
    
private:
    // --- 成员变量 ---
    QDateTime m_date;
    QString m_week;
    double m_maxTemperature;
    double m_minTemperature;
    int m_humidity;
    WeatherType m_weatherType;
    QString m_weatherDescription;
    QString m_dayWeather;
    QString m_nightWeather;
    QString m_dayWind;
    QString m_nightWind;
    QString m_dayPower;
    QString m_nightPower;
};

// ==================== 预报模型类 ====================

class ForecastModel
{
public:
    // --- 构造函数 ---
    ForecastModel();
    
    // --- 基本信息属性 ---
    QString cityId() const;
    void setCityId(const QString& cityId);
    
    QString cityName() const;
    void setCityName(const QString& cityName);
    
    // --- 预报数据管理 ---
    int forecastDays() const;
    DailyForecast* forecastAt(int index) const;
    void addForecast(DailyForecast* forecast);
    void clearForecasts();
    QList<DailyForecast*> forecasts() const;
    
private:
    // --- 成员变量 ---
    QString m_cityId;
    QString m_cityName;
    QList<DailyForecast*> m_forecasts;
};

#endif // FORECASTMODEL_H