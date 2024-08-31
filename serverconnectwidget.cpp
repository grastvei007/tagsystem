#include "serverconnectwidget.h"
#include "ui_serverconnectwidget.h"

#include <QSettings>
#include <QString>
#include <QDialogButtonBox>
#include "taglist.h"

ServerConnectWidget::ServerConnectWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerConnectWidget)
{
    ui->setupUi(this);

    ui->port->setText("5000");
    ui->adress->setText("127.0.0.1");
    ui->autoConnect->setChecked(false);

    auto clientName = TagList::sGetInstance().clientName();
    if(!clientName.isEmpty())
    {
        QSettings settings("june", clientName);
        auto adress = settings.value("serverAdress", "localhost").toString();
        ui->adress->setText(adress);
        auto port = settings.value("serverPort", 5000).toString();
        ui->port->setText(port);
        auto autoConnect = settings.value("serverAutoConnect", false).toBool();
        ui->autoConnect->setChecked(autoConnect);
    }

    connect(this, &QDialog::accepted, this, &ServerConnectWidget::onAccepted);
}

ServerConnectWidget::~ServerConnectWidget()
{
    delete ui;
}

QString ServerConnectWidget::adress() const
{
    return ui->adress->text();
}

int ServerConnectWidget::port() const
{
    return ui->port->text().toInt();
}

bool ServerConnectWidget::autoConnect() const
{
    return ui->autoConnect->checkState() == Qt::CheckState::Checked;
}

void ServerConnectWidget::onAccepted()
{
    auto clientName = TagList::sGetInstance().clientName();
    if(clientName.isEmpty())
        return;

    QSettings settings("june", clientName);
    settings.setValue("serverAdress", adress());
    settings.setValue("serverPort", port());
    settings.setValue("serverAutoConnect", autoConnect());
}
