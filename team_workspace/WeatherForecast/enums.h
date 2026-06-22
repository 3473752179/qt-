#ifndef ENUMS_H
#define ENUMS_H

#include <QString>

// ==================== 成员2：数据管理模块 ====================
// 负责枚举定义、数据模型、城市管理、数据持久化

// ==================== 天气类型枚举 ====================

enum class WeatherType {
    Sunny,
    Cloudy,
    Overcast,
    LightRain,
    HeavyRain,
    Thunderstorm,
    LightSnow,
    HeavySnow,
    Fog,
    Haze,
    Unknown
};

// ==================== 主题模式枚举 ====================

enum class ThemeMode {
    Light,
    Dark
};

// ==================== 温度单位枚举 ====================

enum class TemperatureUnit {
    Celsius,
    Fahrenheit
};

// ==================== 风向枚举 ====================

enum class WindDirection {
    N, NE, E, SE, S, SW, W, NW
};

// ==================== 空气质量等级枚举 ====================

enum class AirQualityLevel {
    Excellent,
    Good,
    Moderate,
    Poor,
    VeryPoor
};

// ==================== 枚举转换函数 ====================

inline QString weatherTypeToString(WeatherType type) {
    switch (type) {
    case WeatherType::Sunny: return "晴";
    case WeatherType::Cloudy: return "多云";
    case WeatherType::Overcast: return "阴";
    case WeatherType::LightRain: return "小雨";
    case WeatherType::HeavyRain: return "大雨";
    case WeatherType::Thunderstorm: return "雷阵雨";
    case WeatherType::LightSnow: return "小雪";
    case WeatherType::HeavySnow: return "大雪";
    case WeatherType::Fog: return "雾";
    case WeatherType::Haze: return "霾";
    default: return "未知";
    }
}

inline WeatherType weatherTypeFromString(const QString& str) {
    QString s = str.trimmed();
    if (s == "晴") return WeatherType::Sunny;
    if (s == "多云") return WeatherType::Cloudy;
    if (s == "阴") return WeatherType::Overcast;
    if (s == "小雨") return WeatherType::LightRain;
    if (s == "大雨") return WeatherType::HeavyRain;
    if (s == "雷阵雨") return WeatherType::Thunderstorm;
    if (s == "小雪") return WeatherType::LightSnow;
    if (s == "大雪") return WeatherType::HeavySnow;
    if (s == "雾") return WeatherType::Fog;
    if (s == "霾") return WeatherType::Haze;
    return WeatherType::Unknown;
}

inline QString themeModeToString(ThemeMode mode) {
    return mode == ThemeMode::Dark ? "深色" : "浅色";
}

#endif // ENUMS_H