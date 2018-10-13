#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readyscene.h"
#include "ui_readyscene.h"
#include "jsondownloader.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslSocket>
#include <QLineEdit>
#include <QStackedWidget>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QJsonArray>
#include <QJsonDocument>

#include <QJsonObject>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , screen(QGuiApplication::primaryScreen())
{
      ui->setupUi(this);
      onStart();
      secondScene->jsnDw->start();
      manager = new QNetworkAccessManager();
        QObject::connect(manager, &QNetworkAccessManager::finished,this, [=](QNetworkReply *reply) {
                if (reply->error()) {
                    qDebug() << reply->errorString();
                    return;
                }

                this->answer = reply->readAll();

                qDebug() << answer;

                QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
                QJsonObject jsonObject = jsonResponse.object();
                QJsonObject a = jsonObject.value(QString("res")).toObject();
                secondScene->token = a.value(QString("token")).toString();
                this -> setCentralWidget(secondScene);
            }
        );
    }

    MainWindow::~MainWindow()
    {
        delete ui;
        delete manager;
    }


void MainWindow::on_enterButton_clicked()
{
    QString login = ui->loginField->text();
    QString password = ui->passwordField->text();

    secondScene->login = login;

    request.setRawHeader("Connection", "keep-alive");
    request.setUrl(QUrl("http://api.torianik.online:5000/login?login=" + login + "&password=" + password));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    manager->get(request);
}

void MainWindow::onStart()
{
    ui->loginField->setPlaceholderText("Login");
    ui->passwordField->setPlaceholderText("Password");
    QPixmap imageSource(":/images/xlvPng.png");
    imageSource = imageSource.scaledToWidth(int(screen->geometry().width()/2));
    ui->imageLabel->setPixmap(imageSource);
    ui->imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->imageLabel->setContentsMargins(int(screen->geometry().height()/27), 0, 0, 0);
    this->ui->verticalLayout_2->setContentsMargins(60,int(screen->geometry().height()/20),60,9);
}
