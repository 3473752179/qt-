#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QApplication::setApplicationName("WeatherForecast");
    QApplication::setOrganizationName("WeatherForecastTeam");
    
    MainWindow w;
    w.show();
    
    return a.exec();
}