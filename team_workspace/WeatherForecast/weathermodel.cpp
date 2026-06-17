#include "weathermodel.h"

// ==================== 成员2：数据管理模块 ====================
// 负责天气数据模型定义

// ==================== 构造函数 ====================

WeatherModel::WeatherModel()
    : m_temperature(0.0), m_feelsLike(0.0), m_humidity(0),
      m_windSpeed(0), m_windDirection(WindDirection::N),
      m_pressure(0), m_visibility(0), m_airQuality(AirQualityLevel::Good),
      m_weatherType(WeatherType::Unknown)
{
}

// ==================== 基本信息属性 ====================

QString WeatherModel::cityId() const
{
    return m_cityId;
}

void WeatherModel::setCityId(const QString& cityId)
{
    m_cityId = cityId;
}

QString WeatherModel::cityName() const
{
    return m_cityName;
}

void WeatherModel::setCityName(const QString& cityName)
{
    m_cityName = cityName;
}

// ==================== 温度属性 ====================

double WeatherModel::temperature() const
{
    return m_temperature;
}

void WeatherModel::setTemperature(double temperature)
{
    m_temperature = temperature;
}

double WeatherModel::feelsLike() const
{
    return m_feelsLike;
}

void WeatherModel::setFeelsLike(double feelsLike)
{
    m_feelsLike = feelsLike;
}

// ==================== 天气状况属性 ====================

int WeatherModel::humidity() const
{
    return m_humidity;
}

void WeatherModel::setHumidity(int humidity)
{
    m_humidity = humidity;
}

int WeatherModel::windSpeed() const
{
    return m_windSpeed;
}

void WeatherModel::setWindSpeed(int windSpeed)
{
    m_windSpeed = windSpeed;
}

WindDirection WeatherModel::windDirection() const
{
    return m_windDirection;
}

void WeatherModel::setWindDirection(WindDirection windDirection)
{
    m_windDirection = windDirection;
}

int WeatherModel::pressure() const
{
    return m_pressure;
}

void WeatherModel::setPressure(int pressure)
{
    m_pressure = pressure;
}

int WeatherModel::visibility() const
{
    return m_visibility;
}

void WeatherModel::setVisibility(int visibility)
{
    m_visibility = visibility;
}

AirQualityLevel WeatherModel::airQuality() const
{
    return m_airQuality;
}

void WeatherModel::setAirQuality(AirQualityLevel airQuality)
{
    m_airQuality = airQuality;
}

WeatherType WeatherModel::weatherType() const
{
    return m_weatherType;
}

void WeatherModel::setWeatherType(WeatherType weatherType)
{
    m_weatherType = weatherType;
}

QString WeatherModel::weatherDescription() const
{
    return m_weatherDescription;
}

void WeatherModel::setWeatherDescription(const QString& weatherDescription)
{
    m_weatherDescription = weatherDescription;
}

// ==================== 更新时间 ====================

QDateTime WeatherModel::updateTime() const
{
    return m_updateTime;
}

void WeatherModel::setUpdateTime(const QDateTime& updateTime)
{
    m_updateTime = updateTime;
}

// ==================== 验证 ====================

bool WeatherModel::isValid() const
{
    return !m_cityId.isEmpty() && m_temperature != 0.0;
}