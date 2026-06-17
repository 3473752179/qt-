#ifndef WEATHERMODEL_H
#define WEATHERMODEL_H

#include <QString>
#include <QDateTime>
#include "enums.h"

// ==================== 成员2：数据管理模块 ====================
// 负责天气数据模型定义

class WeatherModel
{
public:
    // --- 构造函数 ---
    WeatherModel();
    
    // --- 基本信息属性 ---
    QString cityId() const;
    void setCityId(const QString& cityId);
    
    QString cityName() const;
    void setCityName(const QString& cityName);
    
    // --- 温度属性 ---
    double temperature() const;
    void setTemperature(double temperature);
    
    double feelsLike() const;
    void setFeelsLike(double feelsLike);
    
    // --- 天气状况属性 ---
    int humidity() const;
    void setHumidity(int humidity);
    
    int windSpeed() const;
    void setWindSpeed(int windSpeed);
    
    WindDirection windDirection() const;
    void setWindDirection(WindDirection windDirection);
    
    int pressure() const;
    void setPressure(int pressure);
    
    int visibility() const;
    void setVisibility(int visibility);
    
    AirQualityLevel airQuality() const;
    void setAirQuality(AirQualityLevel airQuality);
    
    WeatherType weatherType() const;
    void setWeatherType(WeatherType weatherType);
    
    QString weatherDescription() const;
    void setWeatherDescription(const QString& weatherDescription);
    
    // --- 更新时间 ---
    QDateTime updateTime() const;
    void setUpdateTime(const QDateTime& updateTime);
    
    // --- 验证 ---
    bool isValid() const;
    
private:
    // --- 成员变量 ---
    QString m_cityId;
    QString m_cityName;
    double m_temperature;
    double m_feelsLike;
    int m_humidity;
    int m_windSpeed;
    WindDirection m_windDirection;
    int m_pressure;
    int m_visibility;
    AirQualityLevel m_airQuality;
    WeatherType m_weatherType;
    QString m_weatherDescription;
    QDateTime m_updateTime;
};

#endif // WEATHERMODEL_H