#ifndef JSONDOWNLOADER_H
#define JSONDOWNLOADER_H

#include <QObject>
#include <QJsonArray>
#include <QNetworkRequest>
class QNetworkReply;
class QNetworkAccessManager;
class QNetworkRequest;

class JsonDownloader : public QObject
{
    Q_OBJECT
public:
    explicit JsonDownloader(QObject *parent = nullptr, bool downloadPhotosFlag = true);
    ~JsonDownloader();

    QJsonArray getOrders();
    QJsonArray getWarehouseInfo();
    QJsonObject getDishById(int id);
    QJsonObject getIngredientById(int id);
    QPixmap getPixmap(const QString &name);
    void start();

    bool downloadPhotosFlag;
public slots:
    void onGetUpdateInfo(QNetworkReply *reply);
    void onDownloadedOrders(QNetworkReply *reply);
    void onDownloadedDishes(QNetworkReply *reply);
    void onDownloadedIngredients(QNetworkReply *reply);
    void onDownloadedImages(QNetworkReply *reply);
    void onDownloadedPixmap(QNetworkReply *reply);
    void onDownloadedWarehouseInfo(QNetworkReply *reply);

signals:
    void updateReady();

private:

    void getUpdate();

    void getUpdateInfo();
    void getPixmapFromServer();
    void getImages();
    void getDishes();
    void getIngredientsFromServer();
    void getOrdersFromServer();
    void getWarehouseInfoFromServer();

    bool started;
    bool was_updated;

    QString updateOrdersHash;
    QString updateDishesHash;
    QString updateIngredientsHash;
    QString updateWarehouseHash;
    QString updateImgHash;

    QString currentOrdersHash;
    QString currentDishesHash;
    QString currentIngredientsHash;
    QString currentWarehouseHash;
    QString currentImgHash;

    QJsonArray dishList;
    QJsonArray orderList;
    QJsonArray ingredientList;
    QJsonArray warehouseList;

    QStringList imageNames;
    int cur_img;
    std::map<QString, QPixmap> imgCache;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
};

#endif // JSONDOWNLOADER_H
