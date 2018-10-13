#include "readyscene.h"
#include "mainwindow.h"
#include "ui_readyscene.h"
#include "ui_mainwindow.h"
#include "jsondownloader.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QString>
#include <QSslSocket>
#include <QLineEdit>
#include <QJsonArray>
#include <QQuickView>
#include <QJsonDocument>
#include <QtQuick/QQuickPaintedItem>
#include <QJsonObject>
#include <QGuiApplication>
#include <QScreen>

readyScene::readyScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::readyScene)
    , screen(QGuiApplication::primaryScreen())
    , m_pLocationInfo(nullptr)
{
    connect(jsnDw, SIGNAL(updateReady()), this, SLOT(readyShow()));
    ui->setupUi(this);
    onStart();
    manager = new QNetworkAccessManager();
      QObject::connect(manager, &QNetworkAccessManager::finished,this, [=](QNetworkReply *reply) {
              if (reply->error()) {
                  qDebug() << reply->errorString();
                  return;
              }

              answer = reply->readAll();
              qDebug() << answer;

              if(!deliverd){
                  QJsonObject temp = QJsonDocument::fromJson(answer.toUtf8()).object().value(QString("res")).toObject();
                  QJsonArray order = temp.value(QString("dishes")).toArray();
                  address = temp.value(QString("address")).toString();
                  order_id = temp.value(QString("id")).toString();
                  name = temp.value(QString("name")).toString();
                  coordinats = temp.value(QString("coordinats")).toString();
                  phone = temp.value(QString("phone")).toString();
                  QJsonObject tempObj;
                  QJsonObject tempObjId;
                  for(int i=0;i<order.size();i++){
                      tempObj = order[i].toObject();
                      tempObjId = jsnDw->getDishById(tempObj.value(QString("dish_id")).toInt());
                      orderR = orderR + tempObjId.value(QString("title")).toString() + ": "
                              + QString::number(tempObj.value(QString("number")).toInt());
                      cost += (tempObjId.value(QString("cost")).toDouble()) * tempObj.value(QString("number")).toInt();
                      if(i != order.size() - 1) orderR += "\n";
                  }
                  startLocationAPI();
                  ui->goMaps->setVisible(true);
                  ui->goMaps->setEnabled(true);
                  ui->callButton->setVisible(true);
                  ui->callButton->setEnabled(true);
                  ui->dataField->setVisible(true);
                  ui->dataField->setText( "Клиент:" + ("\n" + name) + "\n" + phone + "\n" + "Адрес:" + "\n" +  address +
                                          "\n" + "Заказ:" + "\n" + orderR + "\nЦена:\n" + QString::number(cost) + " грн.");
                  ui->dataField->setContentsMargins(0, 0, 0, 1);
                  ui->readyButton->setVisible(false);
                  ui->readyButton->setEnabled(false);
                  ui->doneButton->setVisible(true);
                  ui->doneButton->setEnabled(true);
                  ui->label->setVisible(false);
                  deliverd = true;
                  clicked = true;
                  cost = 0;
                  this->ui->verticalLayout->setContentsMargins(60,int(screen->geometry().height()/85),60,int(screen->geometry().height()/85));
              }

          }
      );
}

readyScene::~readyScene()
{
    delete ui;
}

void readyScene::on_readyButton_clicked()
{
    if(clicked){
        qDebug() << token;
        ui->readyButton->setText("Ожидание заказа...");
        clicked = false;
        deliverd = false;
        orderR = "";
        request.setRawHeader("Connection", "keep-alive");
        request.setUrl(QUrl("http://api.torianik.online:5000/ready?token=" + token + "&login=" + login));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        manager->get(request);
    }

}

void readyScene::on_goMaps_clicked()
{
    QString link = "https://www.google.com/maps/dir/?api=1&destination=" + coordinats + "&travelmode=driving";
    QDesktopServices::openUrl(QUrl(link));
}

void readyScene::on_doneButton_clicked()
{
    m_pLocationInfo->stopUpdates();
    this->ui->verticalLayout->setContentsMargins(60,50,60,int(screen->geometry().height()/2.95));
    ui->goMaps->setVisible(false);
    ui->goMaps->setEnabled(false);
    ui->callButton->setVisible(false);
    ui->callButton->setEnabled(false);
    ui->dataField->setVisible(false);
    ui->readyButton->setVisible(true);
    ui->readyButton->setEnabled(true);
    ui->readyButton->setText("Готов к заказу!");
    ui->doneButton->setVisible(false);
    ui->doneButton->setEnabled(false);
    ui->label->setVisible(true);
    request.setRawHeader("Connection", "keep-alive");
    request.setUrl(QUrl("http://api.torianik.online:5000/delivered?login=" + login + "&token=" + token + "&order_id=" + order_id));
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    manager->get(request);
}

void readyScene::startLocationAPI()
{
 if (!m_pLocationInfo)
 {
     m_pLocationInfo = QGeoPositionInfoSource::createDefaultSource(this);
     m_pLocationInfo->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
     connect(m_pLocationInfo, SIGNAL (positionUpdated(QGeoPositionInfo)),
     this, SLOT (positionUpdated(QGeoPositionInfo)));
     m_pLocationInfo->setUpdateInterval(3000);
 }
     m_pLocationInfo->startUpdates();
}

void readyScene::positionUpdated(QGeoPositionInfo geoPositionInfo)
{
 if (geoPositionInfo.isValid())
 {
     QGeoCoordinate geoCoordinate = geoPositionInfo.coordinate();
     qreal latitude = geoCoordinate.latitude();
     qreal longitude = geoCoordinate.longitude();
     request.setUrl(QUrl("http://api.torianik.online:5000/note?&token=" + token + "&login=" +
                         login + "&order_id=" + order_id + "&coordinats=" +
                         QString::number(latitude) + "," + QString::number(longitude) ));
     qDebug() << QString::number(latitude) + "," + QString::number(longitude);
     request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
     manager->get(request);
 }
}


void readyScene::on_callButton_clicked()
{
    QDesktopServices::openUrl(QUrl("tel:" + phone));
}

void readyScene::readyShow(){
    qDebug() << "Updated!";
}

void readyScene::onStart()
{
    this->ui->verticalLayout->setContentsMargins(60,50,60,int(screen->geometry().height()/2.95));
    ui->goMaps->setVisible(false);
    ui->dataField->setVisible(false);
    ui->goMaps->setEnabled(false);
    ui->doneButton->setVisible(false);
    ui->doneButton->setEnabled(false);
    ui->callButton->setVisible(false);
    ui->callButton->setEnabled(false);
}

