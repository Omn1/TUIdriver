#include "jsondownloader.h"
#include <QDebug>
#include <QSslSocket>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslSocket>
#include <QLineEdit>
#include <QStackedWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QPixmapCache>
#include <QStringList>
#include <map>

JsonDownloader::JsonDownloader(QObject *parent, bool downloadPhotosFlag)
    : QObject(parent)
    , downloadPhotosFlag(downloadPhotosFlag)
    , started(0)
    , was_updated(0)
    , cur_img(-1)
    , manager(new QNetworkAccessManager)
{
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
}

JsonDownloader::~JsonDownloader(){};

QJsonArray JsonDownloader::getOrders()
{
    return orderList;
}

QJsonArray JsonDownloader::getWarehouseInfo()
{
    return warehouseList;
}

QJsonObject JsonDownloader::getDishById(int id)
{
    for (int i = 0; i < dishList.size(); i++)
    {
        if (dishList[i].toObject()["id"].toInt() == id)
            return dishList[i].toObject();
    }
    return QJsonObject();
}

QJsonObject JsonDownloader::getIngredientById(int id)
{
    for (int i = 0; i < ingredientList.size(); i++)
    {
        if (ingredientList[i].toObject()["id"].toInt() == id)
            return ingredientList[i].toObject();
    }
    return QJsonObject();
}

QPixmap JsonDownloader::getPixmap(const QString &name)
{
    QPixmap res = imgCache[name];
    return res;
}

void JsonDownloader::start()
{
    if (!started)
    {
        started = 1;
        getUpdateInfo();
    }
}

void JsonDownloader::onGetUpdateInfo(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onGetUpdateInfo(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getUpdateInfo();
        return;
    }
    QByteArray answer = reply->readAll();
    //qDebug() << answer << "\n";
    QJsonObject response = QJsonDocument::fromJson(answer).object();
    //qDebug() << response << "\n";

    QJsonObject json = response["res"].toObject();

    updateImgHash = json["pictures_hash"].toString();
    updateDishesHash = json["dishes_hash"].toString();
    updateOrdersHash = json["orders_hash"].toString();
    updateIngredientsHash = json["ingredient_hash"].toString();
    updateWarehouseHash = json["supplies_hash"].toString();

    getUpdate();
}

void JsonDownloader::onDownloadedOrders(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedOrders(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getOrdersFromServer();
        return;
    }

    QByteArray answer = reply->readAll();

    QJsonObject response = QJsonDocument::fromJson(answer).object();

    orderList = response["res"].toArray();

    getUpdate();
}

void JsonDownloader::onDownloadedDishes(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedDishes(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getDishes();
        return;
    }

    QByteArray answer = reply->readAll();

    QJsonObject response = QJsonDocument::fromJson(answer).object();

    dishList = response["res"].toArray();

    getUpdate();
}

void JsonDownloader::onDownloadedIngredients(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedIngredients(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getIngredientsFromServer();
        return;
    }

    QByteArray answer = reply->readAll();

    QJsonObject response = QJsonDocument::fromJson(answer).object();

    ingredientList = response["res"].toArray();

    getUpdate();
}

void JsonDownloader::onDownloadedImages(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedImages(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getImages();
        return;
    }

    QByteArray answer = reply->readAll();

    QJsonObject response = QJsonDocument::fromJson(answer).object();

    QJsonArray jsonNames = response["res"].toArray();

    qDebug() << jsonNames << "\n";
    imageNames.clear();

    for (int i = 0; i < jsonNames.size(); i++)
    {
        imageNames.append(jsonNames[i].toString());
    }

    cur_img = imageNames.size() - 1;

    imgCache.clear();

    getPixmapFromServer();
}

void JsonDownloader::onDownloadedPixmap(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedPixmap(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getPixmapFromServer();
        return;
    }

    QByteArray answer = reply->readAll();

    QPixmap pixmap = QPixmap::fromImage(QImage::fromData(answer));

    imgCache[imageNames[cur_img]] = pixmap;

    cur_img--;
    getPixmapFromServer();
}

void JsonDownloader::onDownloadedWarehouseInfo(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedWarehouseInfo(QNetworkReply*)));

    if(reply->error())
    {
        qDebug() << reply->errorString() << "\n";
        getWarehouseInfoFromServer();
        return;
    }

    QByteArray answer = reply->readAll();

    QJsonObject response = QJsonDocument::fromJson(answer).object();

    warehouseList = response["res"].toArray();

    getUpdate();
}

void JsonDownloader::getUpdate()
{
    if (downloadPhotosFlag && updateImgHash != currentImgHash)
    {
        getImages();
    }
    else if (updateIngredientsHash != currentIngredientsHash)
    {
        getIngredientsFromServer();
    }
    else if (updateDishesHash != currentDishesHash)
    {
        getDishes();
    }
    else if (updateOrdersHash != currentOrdersHash)
    {
        getOrdersFromServer();
    }
    else if(updateWarehouseHash != currentWarehouseHash){
        getWarehouseInfoFromServer();
    }
    else{
        if(was_updated)
            emit updateReady();
        getUpdateInfo();
    }
}

void JsonDownloader::getUpdateInfo()
{
    //qDebug () << "getUpdateInfo\n";
    currentImgHash = updateImgHash;
    currentDishesHash = updateDishesHash;
    currentOrdersHash = updateOrdersHash;
    was_updated = 0;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onGetUpdateInfo(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/get/state_hashes"));
    manager->get(request);
}

void JsonDownloader::getPixmapFromServer()
{
    qDebug () << "getPixmap\n";
    if (cur_img == -1)
    {
        getUpdate();
        return;
    }
    QString pixmapName = imageNames[cur_img];
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedPixmap(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/public/" + pixmapName));
    manager->get(request);
}

void JsonDownloader::getImages()
{
    qDebug () << "getImages\n";
    currentImgHash = updateImgHash;
    was_updated = 1;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedImages(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/get/images"));
    manager->get(request);
}

void JsonDownloader::getDishes()
{
    qDebug () << "getDishes\n";
    currentDishesHash = updateDishesHash;
    was_updated = 1;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedDishes(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/get/dishes"));
    manager->get(request);
}

void JsonDownloader::getIngredientsFromServer()
{
    qDebug () << "getIngredients\n";
    currentIngredientsHash = updateIngredientsHash;
    was_updated = 1;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedIngredients(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/get/ingredients"));
    manager->get(request);
}

void JsonDownloader::getOrdersFromServer()
{
    qDebug () << "getOrders\n";
    currentOrdersHash = updateOrdersHash;
    was_updated = 1;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedOrders(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/get/orders"));
    manager->get(request);
}

void JsonDownloader::getWarehouseInfoFromServer()
{
    qDebug() << "getWarehouse\n";
    currentWarehouseHash = updateWarehouseHash;
    was_updated = 1;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadedWarehouseInfo(QNetworkReply*)));
    request.setUrl(QUrl("http://api.torianik.online:5000/get/goods"));
    manager->get(request);
}
