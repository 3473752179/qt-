#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

// ==================== 成员1：网络请求模块 ====================
// 负责API调用、数据获取、网络状态管理（纯网络层，不负责数据解析）

class NetworkManager : public QObject
{
    Q_OBJECT
    
public:
    // --- 单例模式 ---
    static NetworkManager& instance();
    
    // --- 网络请求接口 ---
    void fetchCurrentWeather(const QString& cityId, double latitude, double longitude);
    void fetchForecast(const QString& cityId);
    
    // --- API配置 ---
    bool isNetworkAvailable() const;
    void setApiKey(const QString& apiKey);
    QString apiKey() const;
    
signals:
    // --- 数据接收信号（发送原始JSON数据，由成员3解析） ---
    void currentWeatherDataReceived(const QByteArray& data, const QString& cityId);
    void forecastDataReceived(const QByteArray& data, const QString& cityId);
    
    // --- 网络状态信号 ---
    void networkError(const QString& error);
    void networkStatusChanged(bool available);
    void loadingStarted();
    void loadingFinished();
    
private slots:
    // --- 响应处理槽函数 ---
    void onCurrentWeatherReply(QNetworkReply* reply, const QString& cityId);
    void onForecastReply(QNetworkReply* reply, const QString& cityId);
    void onNetworkTimeout();
    void checkNetworkStatus();
    
private:
    // --- 构造函数 ---
    NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();
    
    // --- 成员变量 ---
    QNetworkAccessManager* m_networkManager;
    QString m_apiKey;
    QTimer* m_timeoutTimer;
    QTimer* m_networkCheckTimer;
    bool m_networkAvailable;
};

#endif // NETWORKMANAGER_H
