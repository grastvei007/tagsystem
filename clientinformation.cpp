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

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QStringList>

ClientInformation::ClientInformation(const QString &name) :
    clientName_(name)
{

}

void ClientInformation::setListenToTags(const std::initializer_list<QString> &tagSet)
{
	for(const auto &tag : tagSet)
	{
		tagSet_.push_back(tag);
	}
}


QString ClientInformation::getInfo() const
{
    QStringList ips;
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
             ips.push_back(address.toString());
    }

	QJsonObject json;
	json.insert("name", clientName_);
	json.insert("ip", ips.first());

	if(!tagSet_.isEmpty())
	{
		json.insert("tag_set", QJsonArray::fromStringList(tagSet_));
	}

	QJsonDocument doc(json);

	return QString(doc.toJson());
}
