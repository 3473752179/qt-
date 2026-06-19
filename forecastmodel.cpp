#include "forecastmodel.h"

// ==================== 成员2：数据管理模块 ====================
// 负责预报数据模型定义

// ==================== 每日预报类 ====================

// --- 构造函数 ---
DailyForecast::DailyForecast()
    : m_maxTemperature(0.0), m_minTemperature(0.0), m_humidity(0),
      m_weatherType(WeatherType::Unknown)
{
}

// --- 日期属性 ---
QDateTime DailyForecast::date() const
{
    return m_date;
}

void DailyForecast::setDate(const QDateTime& date)
{
    m_date = date;
}

QString DailyForecast::week() const
{
    return m_week;
}

void DailyForecast::setWeek(const QString& week)
{
    m_week = week;
}

// --- 温度属性 ---
double DailyForecast::maxTemperature() const
{
    return m_maxTemperature;
}

void DailyForecast::setMaxTemperature(double maxTemperature)
{
    m_maxTemperature = maxTemperature;
}

double DailyForecast::minTemperature() const
{
    return m_minTemperature;
}

void DailyForecast::setMinTemperature(double minTemperature)
{
    m_minTemperature = minTemperature;
}

// --- 天气状况属性 ---
int DailyForecast::humidity() const
{
    return m_humidity;
}

void DailyForecast::setHumidity(int humidity)
{
    m_humidity = humidity;
}

WeatherType DailyForecast::weatherType() const
{
    return m_weatherType;
}

void DailyForecast::setWeatherType(WeatherType weatherType)
{
    m_weatherType = weatherType;
}

QString DailyForecast::weatherDescription() const
{
    return m_weatherDescription;
}

void DailyForecast::setWeatherDescription(const QString& weatherDescription)
{
    m_weatherDescription = weatherDescription;
}

// --- 早晚天气 ---
QString DailyForecast::dayWeather() const
{
    return m_dayWeather;
}

void DailyForecast::setDayWeather(const QString& weather)
{
    m_dayWeather = weather;
}

QString DailyForecast::nightWeather() const
{
    return m_nightWeather;
}

void DailyForecast::setNightWeather(const QString& weather)
{
    m_nightWeather = weather;
}

// --- 风向风力 ---
QString DailyForecast::dayWind() const
{
    return m_dayWind;
}

void DailyForecast::setDayWind(const QString& wind)
{
    m_dayWind = wind;
}

QString DailyForecast::nightWind() const
{
    return m_nightWind;
}

void DailyForecast::setNightWind(const QString& wind)
{
    m_nightWind = wind;
}

QString DailyForecast::dayPower() const
{
    return m_dayPower;
}

void DailyForecast::setDayPower(const QString& power)
{
    m_dayPower = power;
}

QString DailyForecast::nightPower() const
{
    return m_nightPower;
}

void DailyForecast::setNightPower(const QString& power)
{
    m_nightPower = power;
}

// ==================== 预报模型类 ====================

// --- 构造函数 ---
ForecastModel::ForecastModel()
{
}

// --- 基本信息属性 ---
QString ForecastModel::cityId() const
{
    return m_cityId;
}

void ForecastModel::setCityId(const QString& cityId)
{
    m_cityId = cityId;
}

QString ForecastModel::cityName() const
{
    return m_cityName;
}

void ForecastModel::setCityName(const QString& cityName)
{
    m_cityName = cityName;
}

// --- 预报数据管理 ---
int ForecastModel::forecastDays() const
{
    return m_forecasts.size();
}

DailyForecast* ForecastModel::forecastAt(int index) const
{
    if (index >= 0 && index < m_forecasts.size()) {
        return m_forecasts[index];
    }
    return nullptr;
}

void ForecastModel::addForecast(DailyForecast* forecast)
{
    if (forecast) {
        m_forecasts.append(forecast);
    }
}

void ForecastModel::clearForecasts()
{
    qDeleteAll(m_forecasts);
    m_forecasts.clear();
}

QList<DailyForecast*> ForecastModel::forecasts() const
{
    return m_forecasts;
}