#ifndef READYSCENE_H
#define READYSCENE_H

#include "jsondownloader.h"

#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

namespace Ui {
class readyScene;
}

class readyScene : public QWidget
{
    Q_OBJECT

public:
    explicit readyScene(QWidget *parent = nullptr);
    ~readyScene();

private slots:
    void on_readyButton_clicked();
    void on_goMaps_clicked();
    void on_doneButton_clicked();
    void positionUpdated(QGeoPositionInfo geoPositionInfo);
    void startLocationAPI();
    void on_callButton_clicked();
    void readyShow();
    void onStart();

public:
    QString token;
    QString login;
    JsonDownloader* jsnDw = new JsonDownloader(nullptr,0);


private:
    Ui::readyScene *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QString answer;
    QString address;
    QString order_id;
    QString  name;
    QString  coordinats;
    QString  phone;
    double  cost = 0;
    QString orderR = "";
    bool deliverd = false;
    bool clicked = true;
    QScreen *screen;


private:
   QGeoPositionInfoSource* m_pLocationInfo;


};

#endif // READYSCENE_H
