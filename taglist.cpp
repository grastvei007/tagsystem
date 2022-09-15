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

#include "clientinformation.h"

TagList& TagList::sGetInstance()
{
    static TagList sTagList;
    return sTagList;
}

TagList::TagList()
{

}

void TagList::freeRide(bool aOn)
{
    freeRideFlag_ = aOn;
}

int TagList::getNumberOfTags() const
{
    return tagByName_.size();
}

Tag* TagList::createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType)
{
    Tag *tag = findByTagName(QString("%1.%2").arg(aSubSystem).arg(aName));
    if(tag)
    {
        return tag;
    }
    // tag does not exist, create it.
    tag = new Tag(aSubSystem, aName, aType);
    tagByName_[tag->getFullName()] = tag;
    tags_.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::valueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);
    if(webSocket_)
        tagsCreateQueue_.push_back(tag);
    qDebug() << "Create tag: " << tag->getFullName() << " (" << tagsCreateQueue_.size() << ")";
    emit tagCreated();
    return tag;
}

Tag *TagList::createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType, QVariant initValue)
{
    Tag *tag = findByTagName(QString("%1.%2").arg(aSubSystem).arg(aName));
    if(tag)
    {
        return tag;
    }
    switch (aType)
    {
    case Tag::eDouble:
    {
        auto value = initValue.toDouble();
        tag = new Tag(aSubSystem, aName, aType, value);
        break;
    }
    case Tag::eInt:
    {
        auto value = initValue.toInt();
        tag = new Tag(aSubSystem, aName, aType, value);
        break;
    }
    case Tag::eBool:
    {
        auto value = initValue.toBool();
        tag = new Tag(aSubSystem, aName, aType, value);
        break;
    }
    default:
        tag = new Tag(aSubSystem, aName, aType);
        break;
    }

    tagByName_[tag->getFullName()] = tag;
    tags_.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::valueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);
    if(webSocket_)
        tagsCreateQueue_.push_back(tag);
    qDebug() << "Create tag: " << tag->getFullName() << " (" << tagsCreateQueue_.size() << ")";
    emit tagCreated();
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


void TagList::toXml(QByteArray &rXml, bool aCreate) const
{
    QXmlStreamWriter stream(&rXml);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("taglist");
    if(aCreate)
        stream.writeStartElement("create");
    else
        stream.writeStartElement("update");

    for(int i=0; i<tags_.size(); ++i)
    {
        tags_[i]->writeToXml(stream);
    }

    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();
}


void TagList::connectToServer(const QString &aAdress, qint16 aPort)
{
    if(clientName_.isEmpty())
        qFatal("Set client name before connecting to server..");
    adress_ = aAdress;
    port_ = aPort;
    QUrl url(QString("ws://%1:%2").arg(aAdress).arg(QString::number(aPort)));
    qDebug() << "Connect to: " << url;
    webSocket_ = new QWebSocket;
    connect(webSocket_, &QWebSocket::connected, this, &TagList::onConnected);
    connect(webSocket_, &QWebSocket::disconnected, this, &TagList::onDisconnected);
    connect(webSocket_, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &TagList::onError);
    webSocket_->open(url);
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
    if(!tagSyncTimer_ && !freeRideFlag_)
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
    emit disconnect();
}


/**
 * @brief TagList::onBinaryDataRecieved
 * @param aMsg
 *
 * Recieve list of tags to create or update.
 */
void TagList::onBinaryDataRecieved(QByteArray aMsg)
{
    //qDebug() << __FUNCTION__ << aMsg;
    QXmlStreamReader stream(aMsg);

    while(!stream.atEnd() && !stream.hasError())
    {
        QXmlStreamReader::TokenType token = stream.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement)
        {
            if(stream.name() == QString("create"))
            {
                while(!(stream.tokenType() == QXmlStreamReader::EndElement &&
                        stream.name() == QString("create")))
                {
                    if(stream.readNext() != QXmlStreamReader::StartElement)
                        continue;

                    if(stream.name() == QString("tag"))
                    {
                        createTag(stream);
                    }
                }
            }
            else if(stream.name() == QString("update"))
            {
                while(!(stream.tokenType() == QXmlStreamReader::EndElement &&
                        stream.name() == QString("update")))
                {
                    if(stream.readNext() != QXmlStreamReader::StartElement)
                        continue;

                    if(stream.name() == QString("tag"))
                    {
                        updateTag(stream);
                    }
                }
            }
        }
    }
}


void TagList::syncTags()
{

    bool update = false;
    bool create = false;

    QByteArray data;
    QXmlStreamWriter stream(&data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("taglist");

    if(tagsCreateQueue_.size() > 0)
    {
        stream.writeStartElement("create");
        for(int i=0; i<tagsCreateQueue_.size(); ++i)
            tagsCreateQueue_[i]->writeToXml(stream);
        tagsCreateQueue_.clear();
        create = true;
        stream.writeEndElement();
    }
    if(tagUpdateQueue_.size() > 0)
    {
        stream.writeStartElement("update");
        for(int i=0; i<tagUpdateQueue_.size(); ++i)
            tagUpdateQueue_[i]->writeToXml(stream);
        tagUpdateQueue_.clear();
        stream.writeEndElement();
        update = true;
    }

    stream.writeEndElement();
    stream.writeEndDocument();

    if(webSocket_ && (update || create))
    {
        qDebug() << "Sending data ";
        webSocket_->sendBinaryMessage(data);
    }
}

Tag* TagList::createTag(QXmlStreamReader &aStream)
{
    QXmlStreamAttributes attribs = aStream.attributes();
    QString subsystem = attribs.value("subsystem").toString();
    QString name = attribs.value("name").toString();
    QString type = attribs.value("type").toString();

    Tag *tag = nullptr;
    // if the tag exist do not create.
    tag = findByTagName(QString("%1.%2").arg(subsystem).arg(name));
    if(tag)
        return nullptr;

    // tag does not exist create it.

    if(type == Tag::toString(Tag::eDouble))
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eDouble);
        tag->setValue(attribs.value("value").toDouble());
    }
    else if(type == Tag::toString(Tag::eInt))
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eInt);
        tag->setValue(attribs.value("value").toInt());
    }
    else if(type == Tag::toString(Tag::eBool))
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eBool);
        tag->setValue(attribs.value("value").toInt() == 1 ? true : false);
    }
    else if(type == Tag::toString(Tag::eString))
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eString);
        tag->setValue(attribs.value("value").toString());
    }
    else if(type == Tag::toString(Tag::eTime))
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eTime);
        tag->setValue(QDateTime::fromMSecsSinceEpoch(attribs.value("value").toLongLong()));
    }
    else
        Q_UNREACHABLE();

    emit tagCreated();
    return tag;
}


Tag* TagList::updateTag(QXmlStreamReader &aStream)
{
    QXmlStreamAttributes attribs = aStream.attributes();
    QString subsystem = attribs.value("subsystem").toString();
    QString name = attribs.value("name").toString();
    QString type = attribs.value("type").toString();
    qint64 timestamp = -1;
    if(attribs.hasAttribute("timestamp"))
    {
        timestamp = attribs.value("timestamp").toLongLong();
    }

    Tag *tag = findByTagName(QString("%1.%2").arg(subsystem).arg(name));
    if(!tag)
    {
        tag = createTag(subsystem, name, Tag::typeFromString(type));
        qDebug() << "Local tag do not exist, create tag:" << QString("%1.%2").arg(subsystem).arg(name);
    }
    if(timestamp > 0 && timestamp < tag->getMsSinceEpoc())
    {
        return tag;
    }

    if(tag->getType() == Tag::eDouble)
    {
        double value = attribs.value("value").toDouble();
        tag->setValue(value, timestamp);
    }
    else if(tag->getType() == Tag::eInt)
    {
        int value = attribs.value("value").toInt();
        tag->setValue(value, timestamp);
    }
    else if(tag->getType() == Tag::eBool)
    {
        bool value = attribs.value("value").toInt() == 1 ? true : false;
        tag->setValue(value, timestamp);
    }
    else if(tag->getType() == Tag::eString)
    {
        QString value = attribs.value("value").toString();
        tag->setValue(value, timestamp);
    }
    else if(tag->getType() == Tag::eTime)
    {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(attribs.value("value").toLongLong());
        tag->setValue(time, timestamp);
    }
    else
        Q_UNREACHABLE();

    return tag;
}

/**
 * @brief TagList::onTagValueChanged
 * @param aTag
 *
 * Handle update if connected.
 */
void TagList::onTagValueChanged(Tag *aTag)
{
    if(!webSocket_)
        return;

    if(tagUpdateQueue_.contains(aTag))
        tagUpdateQueue_.removeAll(aTag);

    tagUpdateQueue_.push_back(aTag);

    if(freeRideFlag_)
        syncTags();
}

