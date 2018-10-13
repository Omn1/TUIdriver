#ifndef TIMER_H
#define TIMER_H

#include <QWidget>

namespace Ui {
class Timer;
}

class Timer : public QWidget
{
    Q_OBJECT

public:
    explicit Timer(QWidget *parent = nullptr);
    ~Timer();

private:
    Ui::Timer *ui;
};

#endif // TIMER_H
