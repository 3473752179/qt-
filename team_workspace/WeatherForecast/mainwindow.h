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
    void onForecastDataReceived(const QByteArray& data, const QString& cityId);
    void onSearchResultsReady(QList<CityModel*> cities);

private:
    // --- UI初始化 ---
    void setupUI();
    void setupConnections();
    void loadFavoriteCities();
    
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
    QLabel *m_cityLabel;
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
    QLabel *m_updateTimeLabel;

    // --- 预报面板控件 ---
    QWidget *m_forecastPanel;
    QHBoxLayout *m_forecastLayout;

    // --- 状态变量 ---
    bool m_isDarkTheme;
};

#endif // MAINWINDOW_H