#ifndef WEATHERANIMATIONWIDGET_H
#define WEATHERANIMATIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include "enums.h"

class WeatherAnimationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherAnimationWidget(QWidget *parent = nullptr);

    void setWeatherType(WeatherType weatherType);
    void setDarkTheme(bool isDark);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void advanceFrame();

private:
    struct Cloud {
        QPointF position;
        QSizeF size;
        qreal speed;
        qreal opacity;
    };

    struct Particle {
        QPointF position;
        QPointF velocity;
        qreal size;
        qreal opacity;
    };

    void resetScene();
    void seedClouds(int count);
    void seedRain(int count);
    void seedSnow(int count);
    void seedFog(int count);
    QPointF randomPoint() const;
    qreal randomBetween(qreal minValue, qreal maxValue) const;
    void drawSky(QPainter &painter);
    void drawSun(QPainter &painter);
    void drawClouds(QPainter &painter);
    void drawRain(QPainter &painter);
    void drawSnow(QPainter &painter);
    void drawFog(QPainter &painter);
    void drawThunder(QPainter &painter);

    WeatherType m_weatherType;
    bool m_isDarkTheme;
    QTimer m_animationTimer;
    QVector<Cloud> m_clouds;
    QVector<Particle> m_rainDrops;
    QVector<Particle> m_snowFlakes;
    QVector<Particle> m_fogBands;
    qreal m_sunPhase;
    int m_lightningFrame;
};

#endif // WEATHERANIMATIONWIDGET_H
