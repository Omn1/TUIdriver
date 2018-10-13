#include "mainwindow.h"
#include "readyscene.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslSocket>
#include <QLineEdit>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow sign_up;

    sign_up.setFixedSize(1080,1920);
    sign_up.show();
    return a.exec();
}

