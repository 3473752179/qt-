#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QGroupBox>
#include <QTableView>
#include <QByteArray>
#include <QTimer>
#include <QScrollArea>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include "citymodel.h"
#include "weathermodel.h"
#include "forecastmodel.h"
#include "enums.h"

// ==================== 成员3：数据解析与界面模块 ====================
// 负责JSON数据解析、UI显示、用户交互、主题切换

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // --- 构造函数 ---
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- 用户交互槽函数 ---
    void onCitySelected(CityModel* city);
    void onAddCityClicked();
    void onRemoveCityClicked();
    void onThemeToggleClicked();
    void onSettingsClicked();
    void onCityListItemClicked(QListWidgetItem* item);
    
    // --- 数据接收槽函数（接收原始JSON数据） ---
    void onWeatherDataReceived(const QByteArray& data, const QString& cityId);
    void onAirQualityDataReceived(const QByteArray& data, const QString& cityId);
    void onForecastDataReceived(const QByteArray& data, const QString& cityId);
    void onSearchResultsReady(QList<CityModel*> cities);

private:
    // --- UI初始化 ---
    void setupUI();
    void setupConnections();
    void loadFavoriteCities();
    void updateCurrentDateTime();
    void updateTrendChart(ForecastModel* forecast);
    void updateCurrentMetricsCharts(const QByteArray& data);
    void applyChartTheme(QChart* chart);
    void applyWeatherBackground(WeatherType weatherType);
    QString weatherBackgroundStyle(WeatherType weatherType) const;
    int parseWindPowerValue(const QString& windPower) const;
    AirQualityLevel parseAirQualityLevel(int aqi) const;
    QString airQualityLevelToString(AirQualityLevel level) const;
    WeatherType parseWeatherCode(int weatherCode) const;
    QString weatherDescriptionFromCode(int weatherCode) const;
    WindDirection parseWindDirection(double windDirectionDegrees) const;
    
    // --- 数据解析 ---
    WeatherType parseChineseWeather(const QString& weather);
    WeatherModel* parseCurrentWeather(const QByteArray& data, const QString& cityId);
    ForecastModel* parseForecast(const QByteArray& data, const QString& cityId);
    
    // --- 数据显示更新 ---
    void updateWeatherDisplay(WeatherModel* weather);
    void updateForecastDisplay(ForecastModel* forecast);
    
    // --- 主题管理 ---
    void applyTheme(bool isDark);

    // --- 侧边栏控件 ---
    QWidget *m_sidebar;
    QLineEdit *m_citySearchLineEdit;
    QPushButton *m_addCityButton;
    QListWidget *m_cityListWidget;
    QPushButton *m_removeCityButton;

    // --- 主内容区域控件 ---
    QWidget *m_mainContent;
    QScrollArea *m_mainScrollArea;
    QLabel *m_cityLabel;
    QLabel *m_currentTimeLabel;
    QLabel *m_currentDateLabel;
    QPushButton *m_themeButton;
    QPushButton *m_settingsButton;

    // --- 天气面板控件 ---
    QWidget *m_weatherPanel;
    QLabel *m_weatherIconLabel;
    QLabel *m_tempLabel;
    QLabel *m_feelsLikeLabel;
    QLabel *m_descLabel;
    QLabel *m_humidityLabel;
    QLabel *m_windLabel;
    QLabel *m_pressureLabel;
    QLabel *m_airQualityLabel;
    QLabel *m_pm25Label;
    QWidget *m_currentMetricsPanel;
    QScrollArea *m_tempMetricsScrollArea;
    QScrollArea *m_airMetricsScrollArea;
    QScrollArea *m_windMetricsScrollArea;
    QChartView *m_tempMetricsChartView;
    QChartView *m_airMetricsChartView;
    QChartView *m_windMetricsChartView;

    // --- 预报面板控件 ---
    QWidget *m_forecastPanel;
    QHBoxLayout *m_forecastLayout;
    QWidget *m_trendPanel;
    QChartView *m_trendChartView;

    // --- 状态变量 ---
    bool m_isDarkTheme;
    WeatherType m_currentWeatherType;
    QTimer *m_clockTimer;
};

#endif // MAINWINDOW_H
