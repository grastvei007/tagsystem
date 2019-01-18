#include "clientinformation.h"

#include <QXmlStreamWriter>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QStringList>

ClientInformation::ClientInformation(const QString &aName) :
    mClientName(aName)
{

}


QString ClientInformation::getInfo() const
{
    QStringList ips;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
             ips.push_back(address.toString());
    }

    QString str;
    QXmlStreamWriter stream(&str);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("client");
    stream.writeTextElement("name", mClientName);
    stream.writeTextElement("ip", ips.first());
    stream.writeEndElement();
    stream.writeEndDocument();


    return str;
}
