#include "serverconnectwidget.h"
#include "ui_serverconnectwidget.h"

ServerConnectWidget::ServerConnectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerConnectWidget)
{
    ui->setupUi(this);
}

ServerConnectWidget::~ServerConnectWidget()
{
    delete ui;
}
