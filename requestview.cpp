#include "requestview.h"
#include "ui_requestview.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QSslSocket>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

requestView::requestView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::requestView)
{
    ui->setupUi(this);
    qDebug() << mainData;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(mainData.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

}

requestView::~requestView()
{
    delete ui;
}
