#ifndef SERVERCONNECTWIDGET_H
#define SERVERCONNECTWIDGET_H

#include "tagsystem_global.h"
#include <QWidget>

namespace Ui {
class ServerConnectWidget;
}

class TAGSYSTEMSHARED_EXPORT ServerConnectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerConnectWidget(QWidget *parent = nullptr);
    ~ServerConnectWidget();

private:
    Ui::ServerConnectWidget *ui;
};

#endif // SERVERCONNECTWIDGET_H
