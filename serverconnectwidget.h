#ifndef SERVERCONNECTWIDGET_H
#define SERVERCONNECTWIDGET_H

#include <QDialog>

namespace Ui {
class ServerConnectWidget;
}

class ServerConnectWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ServerConnectWidget(QWidget *parent = nullptr);
    ~ServerConnectWidget();

    QString adress() const;
    int port() const;
    bool autoConnect() const;

private slots:
    void onAccepted();

private:
    Ui::ServerConnectWidget *ui;
};

#endif // SERVERCONNECTWIDGET_H
