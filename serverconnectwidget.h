#ifndef SERVERCONNECTWIDGET_H
#define SERVERCONNECTWIDGET_H

#include <QWidget>

namespace Ui {
class ServerConnectWidget;
}

class ServerConnectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerConnectWidget(QWidget *parent = nullptr);
    ~ServerConnectWidget();

private:
    Ui::ServerConnectWidget *ui;
};

#endif // SERVERCONNECTWIDGET_H
