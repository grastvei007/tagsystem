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

#include "taglist.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QWebSocket>
#include <QtNetwork>
#include <QTimer>
#include <QDebug>

#include <QJsonArray>
#include <QJsonObject>

#include "clientinformation.h"

TagList& TagList::sGetInstance()
{
    static TagList sTagList;
    return sTagList;
}

int TagList::getNumberOfTags() const
{
    return tagByName_.size();
}

Tag* TagList::createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType)
{
    Tag *tag = findByTagName(QString("%1.%2").arg(aSubSystem, aName));
    if(tag)
    {
        return tag;
    }
    // tag does not exist, create it.
    tag = new Tag(aSubSystem, aName, aType);
    tagByName_[tag->getFullName()] = tag;
    tags_.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);

    qDebug() << "Create tag: " << tag->getFullName() << " (" << tags_.size() << ")";
    emit tagCreated(tags_.count());
    return tag;
}


Tag *TagList::createTag(const QString &subSystem, const QString &name, Tag::Type type, QVariant initValue)
{
    return createTag(subSystem, name, type, initValue, QString());
}


Tag *TagList::createTag(const QString &subSystem, const QString &name, Tag::Type type, QVariant initValue, const QString &description)
{
    auto *tag = findByTagName(QString("%1.%2").arg(subSystem, name));
    if(tag)
        return tag;

    switch(type)
    {
        case Tag::eDouble:
        {
            double value = initValue.toDouble();
            tag = new Tag(subSystem, name, type, value, description);
            break;
        }
        case Tag::eInt:
        {
            auto value = initValue.toInt();
            tag = new Tag(subSystem, name, type, value, description);
            break;
        }
        case Tag::eBool:
        {
            auto value = initValue.toBool();
            tag = new Tag(subSystem, name, type, value, description);
            break;
        }
        case Tag::eString:
        {
            auto value = initValue.toString();
            tag = new Tag(subSystem, name, type, value, description);
            break;
        }
        case Tag::eTime:
        {
            auto value = initValue.toDateTime();
            tag = new Tag(subSystem, name, type, value, description);
            break;
        }
        default:
            tag = new Tag(subSystem, name, type);
            break;
    }

    tagByName_[tag->getFullName()] = tag;
    tags_.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);

    qDebug() << "Create tag: " << tag->getFullName() << " (" << tags_.size() << ")";
    emit tagCreated(tags_.count());
    return tag;
}


Tag* TagList::findByTagName(const QString &aName)
{
    if(!tagByName_.contains(aName))
        return nullptr;

    return tagByName_[aName];
}


Tag* TagList::getTagByIndex(int aIndex)
{
    return tags_.at(aIndex);
}

const QString &TagList::clientName() const
{
    return clientName_;
}

QJsonArray TagList::toJson(bool onlyUpdated) const
{
    QJsonArray array;
    for (auto tag : tags_)
    {
        if (onlyUpdated && !tag->isUpdated())
            continue;

        array.push_back(tag->toJson());
        tag->resetUpdateFlag();
    }
    return array;
}

void TagList::connectToServer(const QString &aAdress, qint16 aPort)
{
    if(clientName_.isEmpty())
        qFatal("Set client name before connecting to server..");
    adress_ = aAdress;
    port_ = aPort;
    QUrl url(QString("ws://%1:%2").arg(aAdress, QString::number(aPort)));
    qDebug() << "Connect to: " << url;
    webSocket_ = new QWebSocket;
    connect(webSocket_, &QWebSocket::connected, this, &TagList::onConnected);
    connect(webSocket_, &QWebSocket::disconnected, this, &TagList::onDisconnected);
    connect(webSocket_, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::errorOccurred), this, &TagList::onError);
    webSocket_->open(url);
}

void TagList::disconnectFromServer()
{
    if(!webSocket_)
        return;
    webSocket_->disconnect();
    webSocket_->deleteLater();
    webSocket_ = nullptr;
    emit serverDisconnected();
    isConnected_ = false;
}

bool TagList::tryToAutoConnect()
{
    if(clientName_.isEmpty())
        return false;
    QSettings settings("june", clientName_);
    auto adress = settings.value("serverAdress", "localhost").toString();
    auto port = settings.value("serverPort", 5000).toInt();
    auto autoConnect = settings.value("serverAutoConnect", false).toBool();
    if(autoConnect)
    {
        connectToServer(adress, port);
        return true;
    }
    return false;
}


void TagList::reconnect()
{
    connectToServer(adress_, port_);
}


void TagList::setClientName(const QString &aName)
{
    clientName_ = aName;
}

void TagList::onError()
{
    QString errorStr = webSocket_->errorString();
    qDebug() << errorStr;
    switch (webSocket_->error())
    {
        case QAbstractSocket::ConnectionRefusedError:
            if(!adress_.isEmpty() && port_ > 1024)
            {
                QTimer::singleShot(1000*60, [this](){
                    qDebug() << "Reconnect..";
                    connectToServer(adress_, port_);});
                return;
            }
            break;

        case QAbstractSocket::RemoteHostClosedError:
            if(!adress_.isEmpty() && port_ > 1024)
            {
                QTimer::singleShot(1000*30, [this](){
                    connectToServer(adress_, port_);});
                return;
            }
            break;
        case QAbstractSocket::HostNotFoundError:
        case QAbstractSocket::SocketAccessError:
        case QAbstractSocket::SocketResourceError:
        case QAbstractSocket::SocketTimeoutError:
        case QAbstractSocket::DatagramTooLargeError:
        case QAbstractSocket::NetworkError:
        case QAbstractSocket::AddressInUseError:
        case QAbstractSocket::SocketAddressNotAvailableError:
        case QAbstractSocket::UnsupportedSocketOperationError:
        case QAbstractSocket::ProxyAuthenticationRequiredError:
        case QAbstractSocket::SslHandshakeFailedError:
        case QAbstractSocket::UnfinishedSocketOperationError:
        case QAbstractSocket::ProxyConnectionRefusedError:
        case QAbstractSocket::ProxyConnectionClosedError:
        case QAbstractSocket::ProxyConnectionTimeoutError:
        case QAbstractSocket::ProxyNotFoundError:
        case QAbstractSocket::ProxyProtocolError:
        case QAbstractSocket::OperationError:
        case QAbstractSocket::SslInternalError:
        case QAbstractSocket::SslInvalidUserDataError:
        case QAbstractSocket::TemporaryError:
        case QAbstractSocket::UnknownSocketError:
            break;

        default:
            Q_UNREACHABLE();
    }

    emit error(errorStr); // emit error if it is not handled.

}


void TagList::onConnected()
{
    ClientInformation cl(clientName_);
    webSocket_->sendTextMessage(cl.getInfo());

    connect(webSocket_, &QWebSocket::binaryMessageReceived, this, &TagList::onBinaryDataRecieved);
   // QByteArray data;
   // toXml(data);
   // mWebSocket->sendBinaryMessage(data);
    if(!tagSyncTimer_)
    {
        tagSyncTimer_ = new QTimer(this);
        tagSyncTimer_->setInterval(1000);
        connect(tagSyncTimer_, &QTimer::timeout, this, &TagList::syncTags);
        tagSyncTimer_->start();
    }
    isConnected_ = true;
    emit connected();
}


void TagList::onDisconnected()
{
    isConnected_ = false;
    emit serverDisconnected();
}


/**
 * @brief TagList::onBinaryDataRecieved
 * @param aMsg
 *
 * Recieve list of tags to create or update.
 */
void TagList::onBinaryDataRecieved(QByteArray aMsg)
{
    auto document = QJsonDocument::fromJson(aMsg);
    auto array = document.array();

    for (const auto &jsonRef : array)
    {
        auto jsonTag = jsonRef.toObject();
        UpdateOrCreateTag(jsonTag);
    }

    if(!initialTagBurstReceived_)
    {
        emit initialTagBurst();
        initialTagBurstReceived_ = true;
    }
}


void TagList::syncTags()
{
    if (!webSocket_)
        return;

    auto list = toJson(true);

    if(!list.empty())
    {
        QJsonDocument document(list);
        webSocket_->sendBinaryMessage(document.toJson());
    }
}

/**
 * @brief TagList::onTagValueChanged
 * @param aTag
 *
 * Handle update if connected.
 */
void TagList::onTagValueChanged(Tag *aTag)
{
    int index = tags_.indexOf(aTag);
    emit valueChangedAtIndex(index);
}

Tag* TagList::UpdateOrCreateTag(const QJsonObject &json)
{
    const QString subsystem = json.value("subsystem").toString();
    const QString name = json.value("name").toString();
    const QString description = json.value("description").toString();
    auto type = Tag::typeFromString(json.value("type").toString());
    auto timestamp = json.value("timestamp").toInteger();
    auto value = json.value("value");

    auto *tag = findByTagName(QString("%1.%2").arg(subsystem, name));
    Tag *createdTag = nullptr;

    switch (type) {
    case Tag::eDouble: {
        if (tag)
            tag->setValue(value.toDouble(), timestamp);
        else
            createdTag = createTag(subsystem, name, Tag::eDouble, value.toDouble(), description);
        break;
    }
    case Tag::eInt: {
        if (tag)
            tag->setValue(value.toInt(), timestamp);
        else
            createdTag = createTag(subsystem, name, Tag::eInt, value.toInt(), description);
        break;
    }
    case Tag::eBool: {
        if (tag)
            tag->setValue(value.toBool(), timestamp);
        else
            createdTag = createTag(subsystem, name, Tag::eBool, value.toBool(), description);
        break;
    }
    case Tag::eString: {
        if (tag)
            tag->setValue(value.toString(), timestamp);
        else
            createdTag = createTag(subsystem, name, Tag::eString, value.toString(), description);
        break;
    }
    case Tag::eTime: {
        auto time = QDateTime::fromMSecsSinceEpoch(value.toInteger());
        if (tag)
            tag->setValue(time, timestamp);
        else
            createdTag = createTag(subsystem, name, Tag::eTime, time, description);
        break;
    }
    default:
        Q_UNREACHABLE();
    }

    // keep the old signals to not break anything
    if (tag)
    {
        tag->resetUpdateFlag(); // update is from server prevent it from ping back
        // update signals
        return tag;
    } else
    {
        emit tagCreated(tags_.count());
        return createdTag;
    }
}
