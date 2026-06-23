#include "weatheranimationwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QRandomGenerator>
#include <QtMath>
#include <algorithm>

WeatherAnimationWidget::WeatherAnimationWidget(QWidget *parent)
    : QWidget(parent),
      m_weatherType(WeatherType::Unknown),
      m_isDarkTheme(false),
      m_sunPhase(0.0),
      m_lightningFrame(0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAutoFillBackground(false);

    m_animationTimer.setInterval(40);
    connect(&m_animationTimer, &QTimer::timeout,
            this, &WeatherAnimationWidget::advanceFrame);
    m_animationTimer.start();
}

void WeatherAnimationWidget::setWeatherType(WeatherType weatherType)
{
    if (m_weatherType == weatherType) {
        return;
    }

    m_weatherType = weatherType;
    resetScene();
    update();
}

void WeatherAnimationWidget::setDarkTheme(bool isDark)
{
    if (m_isDarkTheme == isDark) {
        return;
    }

    m_isDarkTheme = isDark;
    update();
}

void WeatherAnimationWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawSky(painter);

    switch (m_weatherType) {
    case WeatherType::Sunny:
        drawSun(painter);
        drawClouds(painter);
        break;
    case WeatherType::Cloudy:
    case WeatherType::Overcast:
    case WeatherType::Haze:
        drawClouds(painter);
        if (m_weatherType == WeatherType::Haze) {
            drawFog(painter);
        }
        break;
    case WeatherType::LightRain:
    case WeatherType::HeavyRain:
        drawClouds(painter);
        drawRain(painter);
        break;
    case WeatherType::Thunderstorm:
        drawClouds(painter);
        drawRain(painter);
        drawThunder(painter);
        break;
    case WeatherType::LightSnow:
    case WeatherType::HeavySnow:
        drawClouds(painter);
        drawSnow(painter);
        break;
    case WeatherType::Fog:
        drawClouds(painter);
        drawFog(painter);
        break;
    default:
        drawClouds(painter);
        break;
    }
}

void WeatherAnimationWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resetScene();
}

void WeatherAnimationWidget::advanceFrame()
{
    if (width() <= 0 || height() <= 0) {
        return;
    }

    m_sunPhase += 0.03;
    if (m_sunPhase > 6.283185307179586) {
        m_sunPhase = 0.0;
    }

    for (Cloud &cloud : m_clouds) {
        cloud.position.rx() += cloud.speed;
        if (cloud.position.x() - cloud.size.width() > width()) {
            cloud.position.setX(-cloud.size.width() * 1.5);
            cloud.position.setY(randomBetween(height() * 0.05, height() * 0.35));
        }
    }

    for (Particle &drop : m_rainDrops) {
        drop.position += drop.velocity;
        if (drop.position.y() > height() || drop.position.x() > width() + 40.0) {
            drop.position.setX(randomBetween(-40.0, width()));
            drop.position.setY(randomBetween(-height() * 0.35, -10.0));
        }
    }

    for (Particle &flake : m_snowFlakes) {
        flake.position += flake.velocity;
        flake.position.rx() += qSin((flake.position.y() + flake.size) * 0.03) * 0.6;
        if (flake.position.y() > height() + 10.0) {
            flake.position.setX(randomBetween(0.0, width()));
            flake.position.setY(randomBetween(-height() * 0.25, -10.0));
        }
    }

    for (Particle &band : m_fogBands) {
        band.position += band.velocity;
        if (band.position.x() - band.size > width()) {
            band.position.setX(-band.size * 1.2);
            band.position.setY(randomBetween(height() * 0.2, height() * 0.75));
        }
    }

    if (m_weatherType == WeatherType::Thunderstorm) {
        if (m_lightningFrame > 0) {
            --m_lightningFrame;
        } else if (QRandomGenerator::global()->bounded(100) < 3) {
            m_lightningFrame = 2;
        }
    } else {
        m_lightningFrame = 0;
    }

    update();
}

void WeatherAnimationWidget::resetScene()
{
    m_clouds.clear();
    m_rainDrops.clear();
    m_snowFlakes.clear();
    m_fogBands.clear();
    m_lightningFrame = 0;

    int cloudCount = 3;
    if (m_weatherType == WeatherType::Cloudy || m_weatherType == WeatherType::LightRain) {
        cloudCount = 5;
    } else if (m_weatherType == WeatherType::Overcast || m_weatherType == WeatherType::HeavyRain) {
        cloudCount = 6;
    } else if (m_weatherType == WeatherType::Thunderstorm) {
        cloudCount = 7;
    } else if (m_weatherType == WeatherType::Fog || m_weatherType == WeatherType::Haze) {
        cloudCount = 4;
    }
    seedClouds(cloudCount);

    if (m_weatherType == WeatherType::LightRain) {
        seedRain(55);
    } else if (m_weatherType == WeatherType::HeavyRain || m_weatherType == WeatherType::Thunderstorm) {
        seedRain(95);
    }

    if (m_weatherType == WeatherType::LightSnow) {
        seedSnow(45);
    } else if (m_weatherType == WeatherType::HeavySnow) {
        seedSnow(80);
    }

    if (m_weatherType == WeatherType::Fog || m_weatherType == WeatherType::Haze) {
        seedFog(6);
    }
}

void WeatherAnimationWidget::seedClouds(int count)
{
    for (int i = 0; i < count; ++i) {
        Cloud cloud;
        cloud.size = QSizeF(randomBetween(120.0, 260.0), randomBetween(45.0, 90.0));
        cloud.position = QPointF(randomBetween(-80.0, width()), randomBetween(height() * 0.04, height() * 0.34));
        cloud.speed = randomBetween(0.15, 0.5);
        cloud.opacity = randomBetween(0.18, 0.34);
        m_clouds.append(cloud);
    }
}

void WeatherAnimationWidget::seedRain(int count)
{
    for (int i = 0; i < count; ++i) {
        Particle drop;
        drop.position = QPointF(randomBetween(-40.0, width()), randomBetween(-height() * 0.35, height()));
        drop.velocity = QPointF(randomBetween(1.8, 3.4), randomBetween(10.0, 16.0));
        drop.size = randomBetween(10.0, 18.0);
        drop.opacity = randomBetween(0.25, 0.55);
        m_rainDrops.append(drop);
    }
}

void WeatherAnimationWidget::seedSnow(int count)
{
    for (int i = 0; i < count; ++i) {
        Particle flake;
        flake.position = QPointF(randomBetween(0.0, width()), randomBetween(-height() * 0.25, height()));
        flake.velocity = QPointF(randomBetween(-0.4, 0.4), randomBetween(1.0, 2.8));
        flake.size = randomBetween(2.0, 5.0);
        flake.opacity = randomBetween(0.45, 0.85);
        m_snowFlakes.append(flake);
    }
}

void WeatherAnimationWidget::seedFog(int count)
{
    for (int i = 0; i < count; ++i) {
        Particle band;
        band.position = QPointF(randomBetween(-160.0, width()), randomBetween(height() * 0.2, height() * 0.75));
        band.velocity = QPointF(randomBetween(0.25, 0.6), 0.0);
        band.size = randomBetween(180.0, 340.0);
        band.opacity = randomBetween(0.06, 0.16);
        m_fogBands.append(band);
    }
}

QPointF WeatherAnimationWidget::randomPoint() const
{
    return QPointF(randomBetween(0.0, width()), randomBetween(0.0, height()));
}

qreal WeatherAnimationWidget::randomBetween(qreal minValue, qreal maxValue) const
{
    return minValue + QRandomGenerator::global()->generateDouble() * (maxValue - minValue);
}

void WeatherAnimationWidget::drawSky(QPainter &painter)
{
    QColor topColor;
    QColor bottomColor;

    switch (m_weatherType) {
    case WeatherType::Sunny:
        topColor = m_isDarkTheme ? QColor("#27486f") : QColor("#7dc4ff");
        bottomColor = m_isDarkTheme ? QColor("#1e3551") : QColor("#ffe7a8");
        break;
    case WeatherType::Cloudy:
        topColor = m_isDarkTheme ? QColor("#435a77") : QColor("#b7d1e6");
        bottomColor = m_isDarkTheme ? QColor("#28384a") : QColor("#eef5fb");
        break;
    case WeatherType::Overcast:
        topColor = m_isDarkTheme ? QColor("#4b5563") : QColor("#b6c0cb");
        bottomColor = m_isDarkTheme ? QColor("#2f3742") : QColor("#e7ebef");
        break;
    case WeatherType::LightRain:
    case WeatherType::HeavyRain:
    case WeatherType::Thunderstorm:
        topColor = m_isDarkTheme ? QColor("#22384b") : QColor("#7ca3c7");
        bottomColor = m_isDarkTheme ? QColor("#162534") : QColor("#d9e8f3");
        break;
    case WeatherType::LightSnow:
    case WeatherType::HeavySnow:
        topColor = m_isDarkTheme ? QColor("#566779") : QColor("#dcecf8");
        bottomColor = m_isDarkTheme ? QColor("#344455") : QColor("#ffffff");
        break;
    case WeatherType::Fog:
    case WeatherType::Haze:
        topColor = m_isDarkTheme ? QColor("#68727d") : QColor("#d7dddf");
        bottomColor = m_isDarkTheme ? QColor("#3a4147") : QColor("#f2f4f5");
        break;
    default:
        topColor = m_isDarkTheme ? QColor("#38506a") : QColor("#d7ecff");
        bottomColor = m_isDarkTheme ? QColor("#202c3a") : QColor("#f5f9ff");
        break;
    }

    QLinearGradient gradient(rect().topLeft(), rect().bottomRight());
    gradient.setColorAt(0.0, topColor);
    gradient.setColorAt(1.0, bottomColor);
    painter.fillRect(rect(), gradient);
}

void WeatherAnimationWidget::drawSun(QPainter &painter)
{
    const qreal glowRadius = 90.0 + qSin(m_sunPhase) * 12.0;
    const QPointF center(width() * 0.82, height() * 0.16);

    QRadialGradient glow(center, glowRadius * 1.8);
    glow.setColorAt(0.0, QColor(255, 241, 181, m_isDarkTheme ? 120 : 180));
    glow.setColorAt(0.6, QColor(255, 220, 120, m_isDarkTheme ? 50 : 90));
    glow.setColorAt(1.0, QColor(255, 220, 120, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(glow);
    painter.drawEllipse(center, glowRadius * 1.8, glowRadius * 1.8);

    painter.setBrush(QColor(255, 215, 92, 230));
    painter.drawEllipse(center, glowRadius * 0.45, glowRadius * 0.45);
}

void WeatherAnimationWidget::drawClouds(QPainter &painter)
{
    for (const Cloud &cloud : std::as_const(m_clouds)) {
        const QRectF body(cloud.position, cloud.size);
        QColor color = m_isDarkTheme
            ? QColor(255, 255, 255, int(cloud.opacity * 255.0))
            : QColor(255, 255, 255, int(cloud.opacity * 255.0));

        if (m_weatherType == WeatherType::Overcast || m_weatherType == WeatherType::Thunderstorm) {
            color = m_isDarkTheme
                ? QColor(152, 164, 181, int(cloud.opacity * 255.0))
                : QColor(186, 195, 205, int(cloud.opacity * 255.0));
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawEllipse(body.adjusted(body.width() * 0.10, body.height() * 0.20, -body.width() * 0.18, 0));
        painter.drawEllipse(QRectF(body.x() + body.width() * 0.16, body.y(), body.width() * 0.38, body.height() * 0.70));
        painter.drawEllipse(QRectF(body.x() + body.width() * 0.42, body.y() + body.height() * 0.04,
                                   body.width() * 0.34, body.height() * 0.62));
        painter.drawRoundedRect(QRectF(body.x() + body.width() * 0.12, body.y() + body.height() * 0.38,
                                       body.width() * 0.62, body.height() * 0.36),
                                body.height() * 0.18, body.height() * 0.18);
    }
}

void WeatherAnimationWidget::drawRain(QPainter &painter)
{
    QPen pen(QColor(180, 220, 255, m_weatherType == WeatherType::HeavyRain ? 170 : 135));
    pen.setWidthF(m_weatherType == WeatherType::HeavyRain ? 1.8 : 1.3);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    for (const Particle &drop : std::as_const(m_rainDrops)) {
        QColor color = pen.color();
        color.setAlphaF(drop.opacity);
        pen.setColor(color);
        painter.setPen(pen);
        painter.drawLine(drop.position,
                         QPointF(drop.position.x() - drop.velocity.x() * 0.6,
                                 drop.position.y() - drop.size));
    }
}

void WeatherAnimationWidget::drawSnow(QPainter &painter)
{
    painter.setPen(Qt::NoPen);

    for (const Particle &flake : std::as_const(m_snowFlakes)) {
        QColor color(255, 255, 255, int(flake.opacity * 255.0));
        painter.setBrush(color);
        painter.drawEllipse(flake.position, flake.size, flake.size);
    }
}

void WeatherAnimationWidget::drawFog(QPainter &painter)
{
    painter.setPen(Qt::NoPen);

    for (const Particle &band : std::as_const(m_fogBands)) {
        QColor color = m_isDarkTheme
            ? QColor(220, 225, 232, int(band.opacity * 255.0))
            : QColor(255, 255, 255, int(band.opacity * 255.0));
        painter.setBrush(color);
        painter.drawRoundedRect(QRectF(band.position.x(), band.position.y(), band.size, band.size * 0.24),
                                band.size * 0.08, band.size * 0.08);
    }
}

void WeatherAnimationWidget::drawThunder(QPainter &painter)
{
    if (m_lightningFrame <= 0) {
        return;
    }

    const QPointF start(width() * 0.72, height() * 0.12);
    QPainterPath bolt(start);
    bolt.lineTo(width() * 0.66, height() * 0.28);
    bolt.lineTo(width() * 0.73, height() * 0.28);
    bolt.lineTo(width() * 0.61, height() * 0.52);
    bolt.lineTo(width() * 0.67, height() * 0.33);
    bolt.lineTo(width() * 0.60, height() * 0.33);
    bolt.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 246, 170, 210));
    painter.drawPath(bolt);
    painter.fillRect(rect(), QColor(255, 255, 255, 35));
}
