#ifndef REQUESTVIEW_H
#define REQUESTVIEW_H

#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class requestView;
}

class requestView : public QWidget
{
    Q_OBJECT

public:
    explicit requestView(QWidget *parent = nullptr);
    ~requestView();

public:
    QString mainData;

private:
    Ui::requestView *ui;
};

#endif // REQUESTVIEW_H
