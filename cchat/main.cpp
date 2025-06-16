#include "mainwindow.h"
#include <QApplication>
#include <QFile>
/**
*    @file:    main.cpp
*    @brief:
*    @author:  star-cs
*    @date:    2025-05-28  15:40
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 导入qss样式
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    } else {
        qDebug("Open failed");
    }

    // 导入config.ini配置文件
    QString fileName = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);

    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_post = settings.value("GateServer/port").toString();

    gate_url_prefix = "http://" + gate_host + ":" + gate_post;

    MainWindow w;
    w.show();

    return a.exec();
}
