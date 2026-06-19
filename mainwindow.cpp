#include "mainwindow.h"
#include "networkmanager.h"
#include "citymanager.h"
#include "datamanager.h"
#include "enums.h"
#include "weathermodel.h"
#include "forecastmodel.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QScrollArea>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QMouseEvent>
#include <limits>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

// ==================== 成员3：数据解析与界面模块 ====================
// 负责JSON数据解析、UI显示、用户交互、主题切换

// ==================== 构造与初始化 ====================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_isDarkTheme(false),
      m_isDraggingWindow(false),
      m_currentWeatherType(WeatherType::Unknown),
      m_clockTimer(nullptr)
{
    setupUI();
    setupConnections();
    loadFavoriteCities();

    CityModel *defaultCity = DataManager::instance().loadDefaultCity();
    if (defaultCity && defaultCity->isValid()) {
        onCitySelected(defaultCity);
        delete defaultCity;
    }

    updateCurrentDateTime();
    m_clockTimer->start();
}

MainWindow::~MainWindow()
{
}

// ==================== UI界面搭建 ====================

void MainWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setWindowTitle("Weather Forecast");
    resize(900, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- 侧边栏 ---
    m_sidebar = new QWidget();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(200);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(10, 10, 10, 10);
    sidebarLayout->setSpacing(10);

    m_citySearchLineEdit = new QLineEdit();
    m_citySearchLineEdit->setPlaceholderText("搜索城市...");
    sidebarLayout->addWidget(m_citySearchLineEdit);

    m_addCityButton = new QPushButton("添加城市");
    sidebarLayout->addWidget(m_addCityButton);

    m_cityListWidget = new QListWidget();
    sidebarLayout->addWidget(m_cityListWidget, 1);

    m_removeCityButton = new QPushButton("删除城市");
    sidebarLayout->addWidget(m_removeCityButton);

    // --- 主内容区域 ---
    m_mainContent = new QWidget();
    m_mainContent->setObjectName("mainContent");
    m_mainContent->installEventFilter(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(m_mainContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);

    m_weatherAnimationWidget = new WeatherAnimationWidget(m_mainContent);
    m_weatherAnimationWidget->setObjectName("weatherAnimationWidget");
    m_weatherAnimationWidget->lower();

    m_mainScrollArea = new QScrollArea();
    m_mainScrollArea->setObjectName("mainScrollArea");
    m_mainScrollArea->setWidgetResizable(true);
    m_mainScrollArea->setFrameShape(QFrame::NoFrame);
    m_mainScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mainScrollArea->viewport()->setAutoFillBackground(false);
    m_mainScrollArea->setWidget(m_mainContent);

    // --- 标题栏 ---
    m_titleBar = new QWidget();
    m_titleBar->setObjectName("titleBar");
    m_titleBar->setCursor(Qt::OpenHandCursor);
    m_titleBar->installEventFilter(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    m_cityLabel = new QLabel("城市名称");
    m_cityLabel->setCursor(Qt::OpenHandCursor);
    m_cityLabel->installEventFilter(this);
    QFont titleFont = m_cityLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_cityLabel->setFont(titleFont);
    titleLayout->addWidget(m_cityLabel);

    titleLayout->addStretch();

    QWidget *dateTimeWidget = new QWidget();
    dateTimeWidget->setCursor(Qt::OpenHandCursor);
    dateTimeWidget->installEventFilter(this);
    QVBoxLayout *dateTimeLayout = new QVBoxLayout(dateTimeWidget);
    dateTimeLayout->setContentsMargins(0, 0, 0, 0);
    dateTimeLayout->setSpacing(2);

    m_currentTimeLabel = new QLabel("--:--:--");
    m_currentTimeLabel->setCursor(Qt::OpenHandCursor);
    m_currentTimeLabel->installEventFilter(this);
    QFont currentTimeFont = m_currentTimeLabel->font();
    currentTimeFont.setPointSize(18);
    currentTimeFont.setBold(true);
    m_currentTimeLabel->setFont(currentTimeFont);
    m_currentTimeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    dateTimeLayout->addWidget(m_currentTimeLabel);

    m_currentDateLabel = new QLabel("----/--/-- ----");
    m_currentDateLabel->setCursor(Qt::OpenHandCursor);
    m_currentDateLabel->installEventFilter(this);
    QFont currentDateFont = m_currentDateLabel->font();
    currentDateFont.setPointSize(10);
    m_currentDateLabel->setFont(currentDateFont);
    m_currentDateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    dateTimeLayout->addWidget(m_currentDateLabel);

    titleLayout->addWidget(dateTimeWidget);

    m_themeButton = new QPushButton("🌙");
    m_themeButton->setFixedSize(32, 32);
    titleLayout->addWidget(m_themeButton);

    m_settingsButton = new QPushButton("⚙");
    m_settingsButton->setFixedSize(32, 32);
    titleLayout->addWidget(m_settingsButton);

    m_minimizeButton = new QPushButton("-");
    m_minimizeButton->setObjectName("minimizeButton");
    m_minimizeButton->setFixedSize(36, 32);
    titleLayout->addWidget(m_minimizeButton);

    m_maximizeButton = new QPushButton("□");
    m_maximizeButton->setObjectName("maximizeButton");
    m_maximizeButton->setFixedSize(36, 32);
    titleLayout->addWidget(m_maximizeButton);

    m_closeButton = new QPushButton("×");
    m_closeButton->setObjectName("closeButton");
    m_closeButton->setFixedSize(36, 32);
    titleLayout->addWidget(m_closeButton);

    contentLayout->addWidget(m_titleBar);

    // --- 天气面板 ---
    m_weatherPanel = new QWidget();
    m_weatherPanel->setObjectName("weatherPanel");
    QGridLayout *weatherLayout = new QGridLayout(m_weatherPanel);
    weatherLayout->setSpacing(15);

    m_weatherIconLabel = new QLabel("☀");
    QFont iconFont = m_weatherIconLabel->font();
    iconFont.setPointSize(48);
    m_weatherIconLabel->setFont(iconFont);
    weatherLayout->addWidget(m_weatherIconLabel, 0, 0, 3, 1, Qt::AlignCenter);

    m_tempLabel = new QLabel("--°C");
    QFont tempFont = m_tempLabel->font();
    tempFont.setPointSize(36);
    tempFont.setBold(true);
    m_tempLabel->setFont(tempFont);
    weatherLayout->addWidget(m_tempLabel, 0, 1);

    m_feelsLikeLabel = new QLabel("体感 --°C");
    weatherLayout->addWidget(m_feelsLikeLabel, 1, 1);

    m_descLabel = new QLabel("天气状况");
    QFont descFont = m_descLabel->font();
    descFont.setPointSize(14);
    m_descLabel->setFont(descFont);
    weatherLayout->addWidget(m_descLabel, 2, 1);

    m_humidityLabel = new QLabel("湿度: --%");
    weatherLayout->addWidget(m_humidityLabel, 0, 2);

    m_windLabel = new QLabel("风速: -- km/h");
    weatherLayout->addWidget(m_windLabel, 1, 2);

    m_pressureLabel = new QLabel("气压: -- hPa");
    weatherLayout->addWidget(m_pressureLabel, 2, 2);

    m_airQualityLabel = new QLabel("空气质量: --");
    weatherLayout->addWidget(m_airQualityLabel, 3, 1);

    m_pm25Label = new QLabel("PM2.5: -- ug/m3");
    weatherLayout->addWidget(m_pm25Label, 3, 2);

    weatherLayout->setColumnStretch(0, 1);
    weatherLayout->setColumnStretch(1, 1);
    weatherLayout->setColumnStretch(2, 1);

    contentLayout->addWidget(m_weatherPanel);

    m_currentMetricsPanel = new QWidget();
    m_currentMetricsPanel->setObjectName("currentMetricsPanel");
    QVBoxLayout *currentMetricsLayout = new QVBoxLayout(m_currentMetricsPanel);
    currentMetricsLayout->setContentsMargins(0, 0, 0, 0);
    currentMetricsLayout->setSpacing(8);

    QLabel *currentMetricsTitleLabel = new QLabel("实况数据可视化");
    QFont currentMetricsTitleFont = currentMetricsTitleLabel->font();
    currentMetricsTitleFont.setPointSize(12);
    currentMetricsTitleFont.setBold(true);
    currentMetricsTitleLabel->setFont(currentMetricsTitleFont);
    currentMetricsLayout->addWidget(currentMetricsTitleLabel);

    m_tempMetricsChartView = new QChartView();
    m_tempMetricsChartView->setMinimumHeight(260);
    m_tempMetricsChartView->setMinimumWidth(1450);
    m_tempMetricsChartView->setRenderHint(QPainter::Antialiasing);
    m_tempMetricsChartView->setFrameShape(QFrame::NoFrame);
    m_tempMetricsChartView->setStyleSheet("background: transparent; border: none;");
    m_tempMetricsChartView->setBackgroundBrush(Qt::NoBrush);
    m_tempMetricsChartView->viewport()->setAutoFillBackground(false);
    m_tempMetricsChartView->viewport()->setStyleSheet("background: transparent;");
    m_tempMetricsChartView->setAttribute(Qt::WA_TranslucentBackground);
    m_tempMetricsScrollArea = new QScrollArea();
    m_tempMetricsScrollArea->setWidgetResizable(false);
    m_tempMetricsScrollArea->setFrameShape(QFrame::NoFrame);
    m_tempMetricsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_tempMetricsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tempMetricsScrollArea->setMinimumHeight(290);
    m_tempMetricsScrollArea->setStyleSheet("background: transparent; border: none;");
    m_tempMetricsScrollArea->viewport()->setAutoFillBackground(false);
    m_tempMetricsScrollArea->viewport()->setStyleSheet("background: transparent;");
    m_tempMetricsScrollArea->setWidget(m_tempMetricsChartView);
    currentMetricsLayout->addWidget(m_tempMetricsScrollArea);

    m_airMetricsChartView = new QChartView();
    m_airMetricsChartView->setMinimumHeight(260);
    m_airMetricsChartView->setMinimumWidth(1450);
    m_airMetricsChartView->setRenderHint(QPainter::Antialiasing);
    m_airMetricsChartView->setFrameShape(QFrame::NoFrame);
    m_airMetricsChartView->setStyleSheet("background: transparent; border: none;");
    m_airMetricsChartView->setBackgroundBrush(Qt::NoBrush);
    m_airMetricsChartView->viewport()->setAutoFillBackground(false);
    m_airMetricsChartView->viewport()->setStyleSheet("background: transparent;");
    m_airMetricsChartView->setAttribute(Qt::WA_TranslucentBackground);
    m_airMetricsScrollArea = new QScrollArea();
    m_airMetricsScrollArea->setWidgetResizable(false);
    m_airMetricsScrollArea->setFrameShape(QFrame::NoFrame);
    m_airMetricsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_airMetricsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_airMetricsScrollArea->setMinimumHeight(290);
    m_airMetricsScrollArea->setStyleSheet("background: transparent; border: none;");
    m_airMetricsScrollArea->viewport()->setAutoFillBackground(false);
    m_airMetricsScrollArea->viewport()->setStyleSheet("background: transparent;");
    m_airMetricsScrollArea->setWidget(m_airMetricsChartView);
    currentMetricsLayout->addWidget(m_airMetricsScrollArea);

    m_windMetricsChartView = new QChartView();
    m_windMetricsChartView->setMinimumHeight(260);
    m_windMetricsChartView->setMinimumWidth(1450);
    m_windMetricsChartView->setRenderHint(QPainter::Antialiasing);
    m_windMetricsChartView->setFrameShape(QFrame::NoFrame);
    m_windMetricsChartView->setStyleSheet("background: transparent; border: none;");
    m_windMetricsChartView->setBackgroundBrush(Qt::NoBrush);
    m_windMetricsChartView->viewport()->setAutoFillBackground(false);
    m_windMetricsChartView->viewport()->setStyleSheet("background: transparent;");
    m_windMetricsChartView->setAttribute(Qt::WA_TranslucentBackground);
    m_windMetricsScrollArea = new QScrollArea();
    m_windMetricsScrollArea->setWidgetResizable(false);
    m_windMetricsScrollArea->setFrameShape(QFrame::NoFrame);
    m_windMetricsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_windMetricsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_windMetricsScrollArea->setMinimumHeight(290);
    m_windMetricsScrollArea->setStyleSheet("background: transparent; border: none;");
    m_windMetricsScrollArea->viewport()->setAutoFillBackground(false);
    m_windMetricsScrollArea->viewport()->setStyleSheet("background: transparent;");
    m_windMetricsScrollArea->setWidget(m_windMetricsChartView);
    currentMetricsLayout->addWidget(m_windMetricsScrollArea);

    contentLayout->addWidget(m_currentMetricsPanel);

    // --- 预报面板 ---
    m_forecastPanel = new QWidget();
    m_forecastPanel->setObjectName("forecastPanel");
    m_forecastLayout = new QHBoxLayout(m_forecastPanel);
    m_forecastLayout->setSpacing(10);
    m_forecastLayout->setAlignment(Qt::AlignLeft);

    contentLayout->addWidget(m_forecastPanel, 1);

    m_trendPanel = new QWidget();
    m_trendPanel->setObjectName("trendPanel");
    QVBoxLayout *trendLayout = new QVBoxLayout(m_trendPanel);
    trendLayout->setContentsMargins(0, 0, 0, 0);
    trendLayout->setSpacing(8);

    QLabel *trendTitleLabel = new QLabel("天气趋势图");
    QFont trendTitleFont = trendTitleLabel->font();
    trendTitleFont.setPointSize(12);
    trendTitleFont.setBold(true);
    trendTitleLabel->setFont(trendTitleFont);
    trendLayout->addWidget(trendTitleLabel);

    m_trendChartView = new QChartView();
    m_trendChartView->setMinimumHeight(260);
    m_trendChartView->setRenderHint(QPainter::Antialiasing);
    m_trendChartView->setFrameShape(QFrame::NoFrame);
    m_trendChartView->setStyleSheet("background: transparent; border: none;");
    m_trendChartView->setBackgroundBrush(Qt::NoBrush);
    m_trendChartView->viewport()->setAutoFillBackground(false);
    trendLayout->addWidget(m_trendChartView);

    contentLayout->addWidget(m_trendPanel, 1);

    // --- 添加主布局 ---
    mainLayout->addWidget(m_sidebar);
    mainLayout->addWidget(m_mainScrollArea, 1);

    // --- 应用默认主题 ---
    applyTheme(false);
    updateWindowControlButtons();
    updateAnimationBackgroundGeometry();

    m_clockTimer = new QTimer(this);
    m_clockTimer->setInterval(1000);
}

// ==================== 信号槽连接 ====================

void MainWindow::setupConnections()
{
    // --- 用户交互信号 ---
    connect(m_addCityButton, &QPushButton::clicked,
            this, &MainWindow::onAddCityClicked);
    connect(m_removeCityButton, &QPushButton::clicked,
            this, &MainWindow::onRemoveCityClicked);
    connect(m_themeButton, &QPushButton::clicked,
            this, &MainWindow::onThemeToggleClicked);
    connect(m_settingsButton, &QPushButton::clicked,
            this, &MainWindow::onSettingsClicked);
    connect(m_minimizeButton, &QPushButton::clicked,
            this, &MainWindow::onMinimizeClicked);
    connect(m_maximizeButton, &QPushButton::clicked,
            this, &MainWindow::onMaximizeRestoreClicked);
    connect(m_closeButton, &QPushButton::clicked,
            this, &MainWindow::onCloseClicked);
    connect(m_cityListWidget, &QListWidget::itemClicked,
            this, &MainWindow::onCityListItemClicked);
    connect(m_clockTimer, &QTimer::timeout,
            this, &MainWindow::updateCurrentDateTime);

    // --- 数据接收信号（接收原始JSON数据，由本模块解析） ---
    connect(&NetworkManager::instance(), &NetworkManager::currentWeatherDataReceived,
            this, &MainWindow::onWeatherDataReceived);
    connect(&NetworkManager::instance(), &NetworkManager::currentAirQualityDataReceived,
            this, &MainWindow::onAirQualityDataReceived);
    connect(&NetworkManager::instance(), &NetworkManager::forecastDataReceived,
            this, &MainWindow::onForecastDataReceived);
    connect(&NetworkManager::instance(), &NetworkManager::networkError,
            [this](const QString &error) {
                QMessageBox::warning(this, "Network Error", error);
            });

    connect(&CityManager::instance(), &CityManager::searchResultsReady,
            this, &MainWindow::onSearchResultsReady);
}

// ==================== 用户交互处理 ====================

void MainWindow::loadFavoriteCities()
{
    m_cityListWidget->clear();
    QList<CityModel *> cities = CityManager::instance().favoriteCities();
    for (CityModel *city : cities) {
        m_cityListWidget->addItem(city->name());
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_mainContent && event->type() == QEvent::Resize) {
        updateAnimationBackgroundGeometry();
    }

    const bool isTitleBarTarget = watched == m_titleBar
        || watched == m_cityLabel
        || watched == m_currentTimeLabel
        || watched == m_currentDateLabel
        || (watched->isWidgetType() && qobject_cast<QWidget*>(watched)->parentWidget() == m_titleBar);

    if (isTitleBarTarget) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_isDraggingWindow = true;
                m_dragOffset = mouseEvent->globalPos() - frameGeometry().topLeft();
                m_titleBar->setCursor(Qt::ClosedHandCursor);
                return true;
            }
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (m_isDraggingWindow && (mouseEvent->buttons() & Qt::LeftButton)) {
                move(mouseEvent->globalPos() - m_dragOffset);
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_isDraggingWindow = false;
                if (m_titleBar) {
                    m_titleBar->setCursor(Qt::OpenHandCursor);
                }
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        updateWindowControlButtons();
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::updateAnimationBackgroundGeometry()
{
    if (!m_weatherAnimationWidget || !m_mainContent) {
        return;
    }

    m_weatherAnimationWidget->setGeometry(m_mainContent->rect());
    m_weatherAnimationWidget->lower();
}

void MainWindow::updateWindowControlButtons()
{
    if (!m_maximizeButton) {
        return;
    }

    m_maximizeButton->setText(isMaximized() ? "❐" : "□");
}

void MainWindow::updateCurrentDateTime()
{
    static const QStringList kWeekdays = {
        "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"
    };

    const QDateTime currentDateTime = QDateTime::currentDateTime();
    m_currentTimeLabel->setText(currentDateTime.toString("HH:mm:ss"));
    m_currentDateLabel->setText(QString("%1 %2")
                                    .arg(currentDateTime.toString("yyyy-MM-dd"))
                                    .arg(kWeekdays.at(currentDateTime.date().dayOfWeek() % 7)));
}

int MainWindow::parseWindPowerValue(const QString& windPower) const
{
    const QRegularExpression numberPattern("(\\d+)");
    QRegularExpressionMatchIterator iterator = numberPattern.globalMatch(windPower);
    QList<int> values;

    while (iterator.hasNext()) {
        const QRegularExpressionMatch match = iterator.next();
        values.append(match.captured(1).toInt());
    }

    if (values.isEmpty()) {
        return -1;
    }

    if (values.size() == 1) {
        return values.first();
    }

    return (values.first() + values.last()) / 2;
}

AirQualityLevel MainWindow::parseAirQualityLevel(int aqi) const
{
    if (aqi <= 50) return AirQualityLevel::Excellent;
    if (aqi <= 100) return AirQualityLevel::Good;
    if (aqi <= 150) return AirQualityLevel::Moderate;
    if (aqi <= 200) return AirQualityLevel::Poor;
    return AirQualityLevel::VeryPoor;
}

QString MainWindow::airQualityLevelToString(AirQualityLevel level) const
{
    switch (level) {
    case AirQualityLevel::Excellent:
        return "优";
    case AirQualityLevel::Good:
        return "良";
    case AirQualityLevel::Moderate:
        return "轻度污染";
    case AirQualityLevel::Poor:
        return "中度污染";
    case AirQualityLevel::VeryPoor:
        return "重度污染";
    default:
        return "未知";
    }
}

QString MainWindow::weatherBackgroundStyle(WeatherType weatherType) const
{
    const QString panelBackground = m_isDarkTheme
        ? "rgba(15, 23, 42, 0.42)"
        : "rgba(255, 255, 255, 0.68)";
    const QString borderColor = m_isDarkTheme
        ? "rgba(255, 255, 255, 0.10)"
        : "rgba(255, 255, 255, 0.38)";

    return QString(
               "QWidget#mainContent {"
               "background: transparent;"
               "border-radius: 18px;"
               "}"
               "QScrollArea#mainScrollArea { background: transparent; border: none; }"
               "QWidget#qt_scrollarea_viewport { background: transparent; }"
               "QWidget#weatherAnimationWidget { border-radius: 18px; }"
               "QWidget#weatherPanel, QWidget#currentMetricsPanel, QWidget#forecastPanel, QWidget#trendPanel {"
               "background-color: %1;"
               "border: 1px solid %2;"
               "border-radius: 14px;"
               "}"
               ).arg(panelBackground, borderColor);
}

void MainWindow::applyWeatherBackground(WeatherType weatherType)
{
    m_currentWeatherType = weatherType;
    if (m_weatherAnimationWidget) {
        m_weatherAnimationWidget->setWeatherType(weatherType);
    }
    applyTheme(m_isDarkTheme);
}

WeatherType MainWindow::parseWeatherCode(int weatherCode) const
{
    if (weatherCode == 0) return WeatherType::Sunny;
    if (weatherCode == 1 || weatherCode == 2) return WeatherType::Cloudy;
    if (weatherCode == 3) return WeatherType::Overcast;
    if (weatherCode == 45 || weatherCode == 48) return WeatherType::Fog;
    if ((weatherCode >= 51 && weatherCode <= 67) || (weatherCode >= 80 && weatherCode <= 82)) {
        return weatherCode >= 65 ? WeatherType::HeavyRain : WeatherType::LightRain;
    }
    if (weatherCode >= 71 && weatherCode <= 77) {
        return weatherCode >= 75 ? WeatherType::HeavySnow : WeatherType::LightSnow;
    }
    if (weatherCode >= 95) return WeatherType::Thunderstorm;
    return WeatherType::Unknown;
}

QString MainWindow::weatherDescriptionFromCode(int weatherCode) const
{
    switch (weatherCode) {
    case 0: return "晴";
    case 1: return "晴间多云";
    case 2: return "多云";
    case 3: return "阴";
    case 45:
    case 48:
        return "雾";
    case 51:
    case 53:
    case 55:
    case 56:
    case 57:
        return "毛毛雨";
    case 61:
    case 63:
        return "雨";
    case 65:
    case 66:
    case 67:
        return "大雨";
    case 71:
    case 73:
        return "雪";
    case 75:
    case 77:
        return "大雪";
    case 80:
    case 81:
    case 82:
        return "阵雨";
    case 85:
    case 86:
        return "阵雪";
    case 95:
    case 96:
    case 99:
        return "雷暴";
    default:
        return "未知";
    }
}

WindDirection MainWindow::parseWindDirection(double windDirectionDegrees) const
{
    if (windDirectionDegrees < 22.5 || windDirectionDegrees >= 337.5) return WindDirection::N;
    if (windDirectionDegrees < 67.5) return WindDirection::NE;
    if (windDirectionDegrees < 112.5) return WindDirection::E;
    if (windDirectionDegrees < 157.5) return WindDirection::SE;
    if (windDirectionDegrees < 202.5) return WindDirection::S;
    if (windDirectionDegrees < 247.5) return WindDirection::SW;
    if (windDirectionDegrees < 292.5) return WindDirection::W;
    return WindDirection::NW;
}

void MainWindow::applyChartTheme(QChart *chart)
{
    if (!chart) {
        return;
    }

    const QColor textColor = m_isDarkTheme ? QColor("#ffffff") : QColor("#333333");
    const QColor gridColor = m_isDarkTheme ? QColor(255, 255, 255, 72) : QColor(0, 0, 0, 58);
    const QColor plotFillColor = m_isDarkTheme
        ? QColor(9, 15, 28, 44)
        : QColor(255, 255, 255, 76);
    const QColor plotBorderColor = m_isDarkTheme
        ? QColor(255, 255, 255, 26)
        : QColor(255, 255, 255, 90);

    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(plotFillColor));
    chart->setPlotAreaBackgroundPen(QPen(plotBorderColor, 1));
    chart->setTitleBrush(QBrush(textColor));

    if (chart->legend()) {
        chart->legend()->setLabelColor(textColor);
    }

    const auto axes = chart->axes();
    for (QAbstractAxis *axis : axes) {
        axis->setLabelsColor(textColor);
        axis->setLinePenColor(gridColor);
        axis->setGridLineColor(gridColor);
        axis->setMinorGridLineColor(gridColor);
        axis->setTitleBrush(QBrush(textColor));
    }
}

void MainWindow::updateCurrentMetricsCharts(const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }

    const QJsonObject hourly = doc.object().value("hourly").toObject();
    if (hourly.isEmpty()) {
        return;
    }

    const QJsonArray timeArray = hourly.value("time").toArray();
    const QJsonArray temperatureArray = hourly.value("temperature_2m").toArray();
    const QJsonArray apparentTemperatureArray = hourly.value("apparent_temperature").toArray();
    const QJsonArray humidityArray = hourly.value("relative_humidity_2m").toArray();
    const QJsonArray pressureArray = hourly.value("pressure_msl").toArray();
    const QJsonArray windSpeedArray = hourly.value("wind_speed_10m").toArray();
    const QJsonArray windDirectionArray = hourly.value("wind_direction_10m").toArray();

    const int pointCount = qMin(timeArray.size(),
                                qMin(temperatureArray.size(),
                                     qMin(apparentTemperatureArray.size(),
                                          qMin(humidityArray.size(),
                                               qMin(pressureArray.size(),
                                                    qMin(windSpeedArray.size(), windDirectionArray.size()))))));
    if (pointCount <= 0) {
        return;
    }

    QLineSeries *temperatureSeries = new QLineSeries();
    temperatureSeries->setName("温度");
    temperatureSeries->setColor(QColor("#e76f51"));
    temperatureSeries->setPointsVisible(true);

    QLineSeries *apparentTemperatureSeries = new QLineSeries();
    apparentTemperatureSeries->setName("体感温度");
    apparentTemperatureSeries->setColor(QColor("#f4a261"));
    apparentTemperatureSeries->setPointsVisible(true);

    QLineSeries *humiditySeries = new QLineSeries();
    humiditySeries->setName("湿度");
    humiditySeries->setColor(QColor("#3a86ff"));
    humiditySeries->setPointsVisible(true);

    QLineSeries *pressureSeries = new QLineSeries();
    pressureSeries->setName("气压");
    pressureSeries->setColor(QColor("#6d597a"));
    pressureSeries->setPointsVisible(true);

    QLineSeries *windSpeedSeries = new QLineSeries();
    windSpeedSeries->setName("风速");
    windSpeedSeries->setColor(QColor("#2a9d8f"));
    windSpeedSeries->setPointsVisible(true);

    QLineSeries *windDirectionSeries = new QLineSeries();
    windDirectionSeries->setName("风向角度");
    windDirectionSeries->setColor(QColor("#264653"));
    windDirectionSeries->setPointsVisible(true);
    windDirectionSeries->setPen(QPen(QColor("#264653"), 2, Qt::DashLine));

    double minTemp = std::numeric_limits<double>::max();
    double maxTemp = std::numeric_limits<double>::lowest();
    double minPressure = std::numeric_limits<double>::max();
    double maxPressure = std::numeric_limits<double>::lowest();
    double maxWindSpeed = 0.0;
    qint64 minTimestamp = std::numeric_limits<qint64>::max();
    qint64 maxTimestamp = std::numeric_limits<qint64>::lowest();

    for (int i = 0; i < pointCount; ++i) {
        const QString timeText = timeArray.at(i).toString();
        const QDateTime time = QDateTime::fromString(timeText, Qt::ISODate);
        const qint64 timestamp = time.isValid() ? time.toMSecsSinceEpoch() : i;

        const double temperature = temperatureArray.at(i).toDouble();
        const double apparentTemperature = apparentTemperatureArray.at(i).toDouble();
        const double humidity = humidityArray.at(i).toDouble();
        const double pressure = pressureArray.at(i).toDouble();
        const double windSpeed = windSpeedArray.at(i).toDouble();
        const double windDirection = windDirectionArray.at(i).toDouble();

        temperatureSeries->append(timestamp, temperature);
        apparentTemperatureSeries->append(timestamp, apparentTemperature);
        humiditySeries->append(timestamp, humidity);
        pressureSeries->append(timestamp, pressure);
        windSpeedSeries->append(timestamp, windSpeed);
        windDirectionSeries->append(timestamp, windDirection);

        minTemp = qMin(minTemp, qMin(temperature, apparentTemperature));
        maxTemp = qMax(maxTemp, qMax(temperature, apparentTemperature));
        minPressure = qMin(minPressure, pressure);
        maxPressure = qMax(maxPressure, pressure);
        maxWindSpeed = qMax(maxWindSpeed, windSpeed);
        minTimestamp = qMin(minTimestamp, timestamp);
        maxTimestamp = qMax(maxTimestamp, timestamp);
    }

    auto createTimeAxis = [pointCount, minTimestamp, maxTimestamp]() {
        QDateTimeAxis *axis = new QDateTimeAxis();
        axis->setFormat("HH:mm");
        axis->setTickCount(qMin(pointCount, 8));
        axis->setRange(QDateTime::fromMSecsSinceEpoch(minTimestamp),
                       QDateTime::fromMSecsSinceEpoch(maxTimestamp));
        axis->setTitleText("时间");
        return axis;
    };

    {
        QChart *chart = new QChart();
        chart->setTitle("温度与体感温度");
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setMargins(QMargins(28, 20, 28, 18));
        chart->addSeries(temperatureSeries);
        chart->addSeries(apparentTemperatureSeries);

        QDateTimeAxis *axisX = createTimeAxis();
        chart->addAxis(axisX, Qt::AlignBottom);
        temperatureSeries->attachAxis(axisX);
        apparentTemperatureSeries->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("摄氏度");
        axisY->setLabelFormat("%.0f");
        axisY->setTickCount(6);
        axisY->setRange(minTemp - 2.0, maxTemp + 2.0);
        chart->addAxis(axisY, Qt::AlignLeft);
        temperatureSeries->attachAxis(axisY);
        apparentTemperatureSeries->attachAxis(axisY);

        applyChartTheme(chart);
        m_tempMetricsChartView->setChart(chart);
    }

    {
        QChart *chart = new QChart();
        chart->setTitle("湿度与气压");
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setMargins(QMargins(28, 20, 36, 18));
        chart->addSeries(humiditySeries);
        chart->addSeries(pressureSeries);

        QDateTimeAxis *axisX = createTimeAxis();
        chart->addAxis(axisX, Qt::AlignBottom);
        humiditySeries->attachAxis(axisX);
        pressureSeries->attachAxis(axisX);

        QValueAxis *humidityAxis = new QValueAxis();
        humidityAxis->setTitleText("湿度%");
        humidityAxis->setLabelFormat("%.0f");
        humidityAxis->setTickCount(6);
        humidityAxis->setRange(0, 100);
        chart->addAxis(humidityAxis, Qt::AlignLeft);
        humiditySeries->attachAxis(humidityAxis);

        QValueAxis *pressureAxis = new QValueAxis();
        pressureAxis->setTitleText("气压hPa");
        pressureAxis->setLabelFormat("%.0f");
        pressureAxis->setTickCount(6);
        pressureAxis->setRange(minPressure - 2.0, maxPressure + 2.0);
        chart->addAxis(pressureAxis, Qt::AlignRight);
        pressureSeries->attachAxis(pressureAxis);

        applyChartTheme(chart);
        m_airMetricsChartView->setChart(chart);
    }

    {
        QChart *chart = new QChart();
        chart->setTitle("风速与风向");
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setMargins(QMargins(28, 20, 36, 18));
        chart->addSeries(windSpeedSeries);
        chart->addSeries(windDirectionSeries);

        QDateTimeAxis *axisX = createTimeAxis();
        chart->addAxis(axisX, Qt::AlignBottom);
        windSpeedSeries->attachAxis(axisX);
        windDirectionSeries->attachAxis(axisX);

        QValueAxis *windSpeedAxis = new QValueAxis();
        windSpeedAxis->setTitleText("风速km/h");
        windSpeedAxis->setLabelFormat("%.0f");
        windSpeedAxis->setTickCount(6);
        windSpeedAxis->setRange(0, qMax(10.0, maxWindSpeed + 5.0));
        chart->addAxis(windSpeedAxis, Qt::AlignLeft);
        windSpeedSeries->attachAxis(windSpeedAxis);

        QValueAxis *windDirectionAxis = new QValueAxis();
        windDirectionAxis->setTitleText("风向角度");
        windDirectionAxis->setLabelFormat("%.0f");
        windDirectionAxis->setTickCount(5);
        windDirectionAxis->setRange(0, 360);
        chart->addAxis(windDirectionAxis, Qt::AlignRight);
        windDirectionSeries->attachAxis(windDirectionAxis);

        applyChartTheme(chart);
        m_windMetricsChartView->setChart(chart);
    }
}

void MainWindow::updateTrendChart(ForecastModel *forecast)
{
    QChart *chart = new QChart();
    chart->setTitle("未来天气趋势");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QLineSeries *maxTempSeries = new QLineSeries();
    maxTempSeries->setName("最高温");
    maxTempSeries->setColor(QColor("#e76f51"));
    maxTempSeries->setPointsVisible(true);

    QLineSeries *minTempSeries = new QLineSeries();
    minTempSeries->setName("最低温");
    minTempSeries->setColor(QColor("#3a86ff"));
    minTempSeries->setPointsVisible(true);

    QLineSeries *windSeries = new QLineSeries();
    windSeries->setName("风力等级");
    windSeries->setColor(QColor("#2a9d8f"));
    windSeries->setPointsVisible(true);
    windSeries->setPen(QPen(QColor("#2a9d8f"), 2, Qt::DashLine));

    QStringList categories;
    double minTemperature = std::numeric_limits<double>::max();
    double maxTemperature = std::numeric_limits<double>::lowest();
    int maxWindValue = 0;
    bool hasWindSeries = false;

    for (int i = 0; i < qMin(forecast->forecastDays(), 7); ++i) {
        DailyForecast *day = forecast->forecastAt(i);
        if (!day) {
            continue;
        }

        const QString dayName = i == 0 ? "今天" : day->date().toString("MM-dd");
        categories << dayName;

        const double highTemp = day->maxTemperature();
        const double lowTemp = day->minTemperature();
        maxTempSeries->append(i, highTemp);
        minTempSeries->append(i, lowTemp);

        minTemperature = qMin(minTemperature, qMin(highTemp, lowTemp));
        maxTemperature = qMax(maxTemperature, qMax(highTemp, lowTemp));

        const int windValue = parseWindPowerValue(day->dayPower());
        if (windValue >= 0) {
            windSeries->append(i, windValue);
            maxWindValue = qMax(maxWindValue, windValue);
            hasWindSeries = true;
        }
    }

    chart->addSeries(maxTempSeries);
    chart->addSeries(minTempSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    maxTempSeries->attachAxis(axisX);
    minTempSeries->attachAxis(axisX);

    QValueAxis *tempAxis = new QValueAxis();
    tempAxis->setTitleText("温度 (°C)");
    tempAxis->setLabelFormat("%.0f");
    tempAxis->setRange(minTemperature - 2, maxTemperature + 2);
    chart->addAxis(tempAxis, Qt::AlignLeft);
    maxTempSeries->attachAxis(tempAxis);
    minTempSeries->attachAxis(tempAxis);

    if (hasWindSeries) {
        chart->addSeries(windSeries);
        windSeries->attachAxis(axisX);

        QValueAxis *windAxis = new QValueAxis();
        windAxis->setTitleText("风力 (级)");
        windAxis->setLabelFormat("%d");
        windAxis->setRange(0, qMax(3, maxWindValue + 1));
        chart->addAxis(windAxis, Qt::AlignRight);
        windSeries->attachAxis(windAxis);
    } else {
        delete windSeries;
    }

    applyChartTheme(chart);
    m_trendChartView->setChart(chart);
}

void MainWindow::onCitySelected(CityModel *city)
{
    if (!city || !city->isValid())
        return;

    m_cityLabel->setText(city->name());
    CityManager::instance().setCurrentCity(city);
    m_airQualityLabel->setText("空气质量: --");
    m_pm25Label->setText("PM2.5: -- ug/m3");
    NetworkManager::instance().fetchCurrentWeather(city->id(), city->latitude(), city->longitude());
    NetworkManager::instance().fetchCurrentAirQuality(city->id(), city->latitude(), city->longitude());
    NetworkManager::instance().fetchForecast(city->id());
}

void MainWindow::onAddCityClicked()
{
    QString cityName = m_citySearchLineEdit->text().trimmed();
    if (cityName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a city name");
        return;
    }

    CityManager::instance().searchCity(cityName);
}

void MainWindow::onRemoveCityClicked()
{
    QListWidgetItem *item = m_cityListWidget->currentItem();
    if (!item)
        return;

    QString cityName = item->text();
    QList<CityModel *> cities = CityManager::instance().favoriteCities();
    for (CityModel *city : cities) {
        if (city->name() == cityName) {
            CityManager::instance().removeFavoriteCity(city->id());
            loadFavoriteCities();
            return;
        }
    }
}

void MainWindow::onThemeToggleClicked()
{
    m_isDarkTheme = !m_isDarkTheme;
    applyTheme(m_isDarkTheme);
    DataManager::instance().saveThemeMode(m_isDarkTheme ? ThemeMode::Dark : ThemeMode::Light);
}

void MainWindow::onSettingsClicked()
{
    QMessageBox::information(this, "Settings", "Settings Dialog");
}

void MainWindow::onMinimizeClicked()
{
    showMinimized();
}

void MainWindow::onMaximizeRestoreClicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }

    updateWindowControlButtons();
}

void MainWindow::onCloseClicked()
{
    close();
}

void MainWindow::onCityListItemClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    QString cityName = item->text();
    QList<CityModel *> cities = CityManager::instance().favoriteCities();
    for (CityModel *city : cities) {
        if (city->name() == cityName) {
            onCitySelected(city);
            return;
        }
    }
}

void MainWindow::onSearchResultsReady(QList<CityModel *> cities)
{
    if (cities.isEmpty()) {
        QMessageBox::information(this, "Search", "No cities found");
        return;
    }

    CityModel *firstCity = cities.first();
    CityManager::instance().addFavoriteCity(firstCity);
    loadFavoriteCities();
    onCitySelected(firstCity);
    m_citySearchLineEdit->clear();

    qDeleteAll(cities);
}

// ==================== 数据解析 ====================

WeatherType MainWindow::parseChineseWeather(const QString& weather)
{
    QString w = weather.trimmed();
    if (w.contains("晴")) return WeatherType::Sunny;
    if (w.contains("多云")) return WeatherType::Cloudy;
    if (w.contains("阴")) return WeatherType::Overcast;
    if (w.contains("雨")) {
        if (w.contains("大") || w.contains("暴")) return WeatherType::HeavyRain;
        if (w.contains("雷")) return WeatherType::Thunderstorm;
        return WeatherType::LightRain;
    }
    if (w.contains("雪")) {
        if (w.contains("大") || w.contains("暴")) return WeatherType::HeavySnow;
        return WeatherType::LightSnow;
    }
    if (w.contains("雾")) return WeatherType::Fog;
    if (w.contains("霾")) return WeatherType::Haze;
    return WeatherType::Unknown;
}

WeatherModel* MainWindow::parseCurrentWeather(const QByteArray& data, const QString& cityId)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return nullptr;

    QJsonObject obj = doc.object();
    QJsonObject current = obj["current"].toObject();
    if (current.isEmpty()) {
        qWarning() << "Open-Meteo API error: missing current field";
        return nullptr;
    }

    WeatherModel* weather = new WeatherModel();
    weather->setCityId(cityId);

    CityModel *currentCity = CityManager::instance().currentCity();
    if (currentCity && currentCity->id() == cityId) {
        weather->setCityName(currentCity->name());
    } else {
        weather->setCityName(cityId);
    }

    const double temperature = current["temperature_2m"].toDouble();
    const double apparentTemperature = current["apparent_temperature"].toDouble(temperature);
    const int humidity = current["relative_humidity_2m"].toInt();
    const int windSpeed = qRound(current["wind_speed_10m"].toDouble());
    const double windDirectionDegrees = current["wind_direction_10m"].toDouble();
    const int pressure = qRound(current["pressure_msl"].toDouble());
    const int weatherCode = current["weather_code"].toInt(-1);

    weather->setTemperature(temperature);
    weather->setFeelsLike(apparentTemperature);
    weather->setHumidity(humidity);
    weather->setWindSpeed(windSpeed);
    weather->setWindDirection(parseWindDirection(windDirectionDegrees));
    weather->setPressure(pressure);
    weather->setWeatherDescription(weatherDescriptionFromCode(weatherCode));
    weather->setWeatherType(parseWeatherCode(weatherCode));
    weather->setUpdateTime(QDateTime::currentDateTime());

    qDebug() << "Parsed weather:" << weather->cityName() << weather->temperature() << "°C";

    return weather;
}

ForecastModel* MainWindow::parseForecast(const QByteArray& data, const QString& cityId)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return nullptr;

    QJsonObject obj = doc.object();

    if (obj["status"].toString() != "1") {
        qWarning() << "Amap API error:" << obj["info"].toString();
        return nullptr;
    }

    QJsonArray forecasts = obj["forecasts"].toArray();
    if (forecasts.isEmpty()) return nullptr;

    QJsonObject forecastObj = forecasts[0].toObject();

    ForecastModel* forecast = new ForecastModel();
    forecast->setCityId(cityId);
    forecast->setCityName(forecastObj["city"].toString());

    QJsonArray casts = forecastObj["casts"].toArray();
    for (int i = 0; i < qMin(casts.size(), 7); ++i) {
        QJsonObject dayObj = casts[i].toObject();
        DailyForecast* day = new DailyForecast();

        QString dateStr = dayObj["date"].toString();
        QStringList dateParts = dateStr.split("-");
        if (dateParts.size() == 3) {
            day->setDate(QDate(dateParts[0].toInt(), dateParts[1].toInt(), dateParts[2].toInt()).startOfDay());
        } else {
            day->setDate(QDateTime::currentDateTime().addDays(i));
        }

        day->setWeek(dayObj["week"].toString());

        QString dayTempStr = dayObj["daytemp"].toString();
        QString nightTempStr = dayObj["nighttemp"].toString();
        day->setMaxTemperature(dayTempStr.toDouble());
        day->setMinTemperature(nightTempStr.toDouble());

        day->setDayWeather(dayObj["dayweather"].toString());
        day->setNightWeather(dayObj["nightweather"].toString());

        day->setDayWind(dayObj["daywind"].toString());
        day->setNightWind(dayObj["nightwind"].toString());

        day->setDayPower(dayObj["daypower"].toString());
        day->setNightPower(dayObj["nightpower"].toString());

        day->setWeatherDescription(dayObj["dayweather"].toString());
        day->setWeatherType(parseChineseWeather(dayObj["dayweather"].toString()));

        forecast->addForecast(day);
    }

    qDebug() << "Parsed forecast:" << forecast->cityName() << forecast->forecastDays() << "days";

    return forecast;
}

// ==================== 数据接收与显示 ====================

void MainWindow::onWeatherDataReceived(const QByteArray& data, const QString& cityId)
{
    WeatherModel* weather = parseCurrentWeather(data, cityId);
    if (weather) {
        updateWeatherDisplay(weather);
        updateCurrentMetricsCharts(data);
        DataManager::instance().cacheWeatherData(weather->cityId(), weather);
        delete weather;
    }
}

void MainWindow::onAirQualityDataReceived(const QByteArray& data, const QString& cityId)
{
    CityModel *currentCity = CityManager::instance().currentCity();
    if (!currentCity || currentCity->id() != cityId) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }

    const QJsonObject current = doc.object().value("current").toObject();
    if (current.isEmpty()) {
        return;
    }

    const int aqi = qRound(current.value("us_aqi").toDouble(-1));
    const double pm25 = current.value("pm2_5").toDouble(-1.0);

    if (aqi >= 0) {
        const AirQualityLevel level = parseAirQualityLevel(aqi);
        m_airQualityLabel->setText(QString("空气质量: %1 (AQI %2)")
                                       .arg(airQualityLevelToString(level))
                                       .arg(aqi));
    }

    if (pm25 >= 0.0) {
        m_pm25Label->setText(QString("PM2.5: %1 ug/m3").arg(pm25, 0, 'f', 1));
    }
}

void MainWindow::onForecastDataReceived(const QByteArray& data, const QString& cityId)
{
    ForecastModel* forecast = parseForecast(data, cityId);
    if (forecast) {
        updateForecastDisplay(forecast);
        updateTrendChart(forecast);
        DataManager::instance().cacheForecastData(forecast->cityId(), forecast);
        delete forecast;
    }
}

void MainWindow::updateWeatherDisplay(WeatherModel *weather)
{
    m_tempLabel->setText(QString("%1°C").arg(weather->temperature(), 0, 'f', 1));
    m_feelsLikeLabel->setText(QString("体感 %1°C").arg(weather->feelsLike(), 0, 'f', 1));
    m_humidityLabel->setText(QString("湿度: %1%").arg(weather->humidity()));
    m_windLabel->setText(QString("风速: %1 km/h").arg(weather->windSpeed()));
    m_pressureLabel->setText(QString("气压: %1 hPa").arg(weather->pressure()));
    m_descLabel->setText(weather->weatherDescription().isEmpty()
                             ? weatherTypeToString(weather->weatherType())
                             : weather->weatherDescription());

    QString weatherIcon = "☀";
    switch (weather->weatherType()) {
    case WeatherType::Sunny:
        weatherIcon = "☀";
        break;
    case WeatherType::Cloudy:
        weatherIcon = "☁";
        break;
    case WeatherType::Overcast:
        weatherIcon = "☁☁";
        break;
    case WeatherType::LightRain:
        weatherIcon = "🌧";
        break;
    case WeatherType::HeavyRain:
        weatherIcon = "⛈";
        break;
    case WeatherType::Thunderstorm:
        weatherIcon = "⚡";
        break;
    case WeatherType::LightSnow:
        weatherIcon = "🌨";
        break;
    case WeatherType::HeavySnow:
        weatherIcon = "❄";
        break;
    case WeatherType::Fog:
        weatherIcon = "🌫";
        break;
    default:
        weatherIcon = "☀";
    }
    m_weatherIconLabel->setText(weatherIcon);
    applyWeatherBackground(weather->weatherType());
}

void MainWindow::updateForecastDisplay(ForecastModel *forecast)
{
    QLayoutItem *item;
    while ((item = m_forecastLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < qMin(forecast->forecastDays(), 7); ++i) {
        DailyForecast *day = forecast->forecastAt(i);
        if (day) {
            QWidget *card = new QWidget();
            QVBoxLayout *layout = new QVBoxLayout(card);
            layout->setAlignment(Qt::AlignCenter);
            layout->setSpacing(5);

            QString dayName = i == 0 ? "今天" : day->date().toString("MM-dd");
            QLabel *dateLabel = new QLabel(dayName);
            dateLabel->setAlignment(Qt::AlignCenter);
            QFont dateFont = dateLabel->font();
            dateFont.setPointSize(12);
            dateLabel->setFont(dateFont);

            QString weekStr = day->week();
            if (!weekStr.isEmpty()) {
                QLabel *weekLabel = new QLabel(weekStr);
                weekLabel->setAlignment(Qt::AlignCenter);
                QFont weekFont = weekLabel->font();
                weekFont.setPointSize(10);
                weekLabel->setFont(weekFont);
                layout->addWidget(weekLabel);
            }

            QString icon = "☀";
            switch (day->weatherType()) {
            case WeatherType::Sunny:
                icon = "☀";
                break;
            case WeatherType::Cloudy:
                icon = "☁";
                break;
            case WeatherType::Overcast:
                icon = "☁☁";
                break;
            case WeatherType::LightRain:
                icon = "🌧";
                break;
            case WeatherType::HeavyRain:
                icon = "⛈";
                break;
            case WeatherType::Thunderstorm:
                icon = "⚡";
                break;
            case WeatherType::LightSnow:
                icon = "🌨";
                break;
            case WeatherType::HeavySnow:
                icon = "❄";
                break;
            case WeatherType::Fog:
                icon = "🌫";
                break;
            default:
                icon = "☀";
            }
            QLabel *iconLabel = new QLabel(icon);
            iconLabel->setAlignment(Qt::AlignCenter);
            QFont iconFont = iconLabel->font();
            iconFont.setPointSize(24);
            iconLabel->setFont(iconFont);

            QLabel *weatherLabel = new QLabel(day->dayWeather() + "/" + day->nightWeather());
            weatherLabel->setAlignment(Qt::AlignCenter);
            QFont weatherFont = weatherLabel->font();
            weatherFont.setPointSize(11);
            weatherLabel->setFont(weatherFont);

            QLabel *tempLabel = new QLabel(QString("%1°/%2°").arg(day->maxTemperature(), 0, 'f', 0).arg(day->minTemperature(), 0, 'f', 0));
            tempLabel->setAlignment(Qt::AlignCenter);
            QFont tempFont = tempLabel->font();
            tempFont.setPointSize(14);
            tempFont.setBold(true);
            tempLabel->setFont(tempFont);

            QString windInfo = "";
            if (!day->dayWind().isEmpty()) {
                windInfo = day->dayWind() + day->dayPower();
            }
            QLabel *windLabel = new QLabel(windInfo);
            windLabel->setAlignment(Qt::AlignCenter);
            QFont windFont = windLabel->font();
            windFont.setPointSize(10);
            windLabel->setFont(windFont);

            layout->addWidget(dateLabel);
            layout->addWidget(iconLabel);
            layout->addWidget(weatherLabel);
            layout->addWidget(tempLabel);
            layout->addWidget(windLabel);

            card->setLayout(layout);
            card->setMinimumWidth(100);
            card->setStyleSheet("background-color: rgba(255,255,255,0.1); border-radius: 8px; padding: 10px;");
            m_forecastLayout->addWidget(card);
        }
    }
}

// ==================== 主题管理 ====================

void MainWindow::applyTheme(bool isDark)
{
    m_isDarkTheme = isDark;

    if (m_weatherAnimationWidget) {
        m_weatherAnimationWidget->setDarkTheme(isDark);
        m_weatherAnimationWidget->setWeatherType(m_currentWeatherType);
    }

    QString baseStyle;
    if (isDark) {
        baseStyle =
            "QMainWindow { background-color: #1a1a1a; }"
            "QWidget { color: #ffffff; }"
            "QWidget#sidebar { background-color: #202833; border-radius: 12px; }"
            "QWidget#titleBar { background: transparent; }"
            "QLineEdit { background-color: #333; color: #fff; border: 1px solid #555; padding: 5px; border-radius: 3px; }"
            "QListWidget { background-color: #333; border: 1px solid #555; color: #ffffff; }"
            "QListWidget::item:hover { background-color: #4a5568; }"
            "QListWidget::item:selected { background-color: #4a90d9; color: white; }"
            "QPushButton { background-color: #4a90d9; color: white; border: none; padding: 6px 12px; border-radius: 3px; }"
            "QPushButton:hover { background-color: #357abd; }"
            "QPushButton#minimizeButton, QPushButton#maximizeButton, QPushButton#closeButton {"
            "background-color: transparent; border-radius: 4px; min-width: 36px; padding: 0px; }"
            "QPushButton#minimizeButton:hover, QPushButton#maximizeButton:hover { background-color: rgba(255, 255, 255, 0.12); }"
            "QPushButton#closeButton:hover { background-color: #e81123; }"
            "QLabel { color: #ffffff; }";
        m_themeButton->setText("☀");
    } else {
        baseStyle =
            "QMainWindow { background-color: #f5f5f5; }"
            "QWidget { color: #333333; }"
            "QWidget#sidebar { background-color: #f2f5f9; border: 1px solid #e4ebf2; border-radius: 12px; }"
            "QWidget#titleBar { background: transparent; }"
            "QLineEdit { background-color: #ffffff; border: 1px solid #ccc; padding: 5px; border-radius: 3px; }"
            "QListWidget { background-color: #ffffff; border: 1px solid #e0e0e0; }"
            "QListWidget::item:hover { background-color: #e8f0fe; }"
            "QListWidget::item:selected { background-color: #4a90d9; color: white; }"
            "QPushButton { background-color: #4a90d9; color: white; border: none; padding: 6px 12px; border-radius: 3px; }"
            "QPushButton:hover { background-color: #357abd; }"
            "QPushButton#minimizeButton, QPushButton#maximizeButton, QPushButton#closeButton {"
            "background-color: transparent; color: #333333; border-radius: 4px; min-width: 36px; padding: 0px; }"
            "QPushButton#minimizeButton:hover, QPushButton#maximizeButton:hover { background-color: rgba(0, 0, 0, 0.08); }"
            "QPushButton#closeButton:hover { background-color: #e81123; color: white; }"
            "QLabel { color: #333333; }";
        m_themeButton->setText("🌙");
    }

    setStyleSheet(baseStyle + weatherBackgroundStyle(m_currentWeatherType));

    if (m_trendChartView && m_trendChartView->chart()) {
        applyChartTheme(m_trendChartView->chart());
    }
    if (m_tempMetricsChartView && m_tempMetricsChartView->chart()) {
        applyChartTheme(m_tempMetricsChartView->chart());
    }
    if (m_airMetricsChartView && m_airMetricsChartView->chart()) {
        applyChartTheme(m_airMetricsChartView->chart());
    }
    if (m_windMetricsChartView && m_windMetricsChartView->chart()) {
        applyChartTheme(m_windMetricsChartView->chart());
    }
}
