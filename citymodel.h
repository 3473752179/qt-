#ifndef CITYMODEL_H
#define CITYMODEL_H

#include <QString>

// ==================== 成员2：数据管理模块 ====================
// 负责城市数据模型定义

class CityModel
{
public:
    // --- 构造函数 ---
    CityModel();
    CityModel(const QString& id, const QString& name, const QString& country,
              double latitude, double longitude);
    
    // --- 属性获取 ---
    QString id() const;
    QString name() const;
    QString country() const;
    double latitude() const;
    double longitude() const;
    
    // --- 属性设置 ---
    void setId(const QString& id);
    void setName(const QString& name);
    void setCountry(const QString& country);
    void setLatitude(double latitude);
    void setLongitude(double longitude);
    
    // --- 验证 ---
    bool isValid() const;
    
private:
    // --- 成员变量 ---
    QString m_id;
    QString m_name;
    QString m_country;
    double m_latitude;
    double m_longitude;
};

#endif // CITYMODEL_H