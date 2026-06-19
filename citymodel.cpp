#include "citymodel.h"

// ==================== 成员2：数据管理模块 ====================
// 负责城市数据模型定义

// ==================== 构造函数 ====================

CityModel::CityModel()
    : m_latitude(0.0), m_longitude(0.0)
{
}

CityModel::CityModel(const QString& id, const QString& name, const QString& country,
                     double latitude, double longitude)
    : m_id(id), m_name(name), m_country(country),
      m_latitude(latitude), m_longitude(longitude)
{
}

// ==================== 属性获取 ====================

QString CityModel::id() const
{
    return m_id;
}

QString CityModel::name() const
{
    return m_name;
}

QString CityModel::country() const
{
    return m_country;
}

double CityModel::latitude() const
{
    return m_latitude;
}

double CityModel::longitude() const
{
    return m_longitude;
}

// ==================== 属性设置 ====================

void CityModel::setId(const QString& id)
{
    m_id = id;
}

void CityModel::setName(const QString& name)
{
    m_name = name;
}

void CityModel::setCountry(const QString& country)
{
    m_country = country;
}

void CityModel::setLatitude(double latitude)
{
    m_latitude = latitude;
}

void CityModel::setLongitude(double longitude)
{
    m_longitude = longitude;
}

// ==================== 验证 ====================

bool CityModel::isValid() const
{
    return !m_id.isEmpty() && !m_name.isEmpty();
}