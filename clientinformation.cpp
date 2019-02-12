/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/

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
