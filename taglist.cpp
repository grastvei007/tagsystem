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

TagList::TagList() :
    mWebSocket(nullptr),
    mTagSyncTimer(nullptr),
    mFreeRideFlag(false),
    mAutoconnectOnBroadcastFlag(false),
    mIsConnected(false)
{
    mUdpSocket = new QUdpSocket(this);
    mUdpSocket->bind(45454, QUdpSocket::ShareAddress);
    connect(mUdpSocket, &QUdpSocket::readyRead, this, &TagList::onRecieveDatagrams);
}

void TagList::freeRide(bool aOn)
{
    mFreeRideFlag = aOn;
}

int TagList::getNumberOfTags() const
{
    return mTagByName.size();
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
    mTagByName[tag->getFullName()] = tag;
    mTags.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::valueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);
    if(mWebSocket)
        mTagsCreateQueue.push_back(tag);
    qDebug() << "Create tag: " << tag->getFullName() << " (" << mTagsCreateQueue.size() << ")";
    emit tagCreated();
    return tag;
}


Tag* TagList::findByTagName(const QString &aName)
{
    if(!mTagByName.contains(aName))
        return nullptr;

    return mTagByName[aName];
}


Tag* TagList::getTagByIndex(int aIndex)
{
    return mTags.at(aIndex);
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

    for(int i=0; i<mTags.size(); ++i)
    {
        mTags[i]->writeToXml(stream);
    }

    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();
}


void TagList::connectToServer(const QString &aAdress, qint16 aPort)
{
    if(mClientName.isEmpty())
        qFatal("Set client name before connecting to server..");
    mAdress = aAdress;
    mPort = aPort;
    QUrl url(QString("ws://%1:%2").arg(aAdress).arg(QString::number(aPort)));
    qDebug() << "Connect to: " << url;
    mWebSocket = new QWebSocket;
    connect(mWebSocket, &QWebSocket::connected, this, &TagList::onConnected);
    connect(mWebSocket, &QWebSocket::disconnected, this, &TagList::onDisconnected);
    connect(mWebSocket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &TagList::onError);
    mWebSocket->open(url);
}


void TagList::reconnect()
{
    connectToServer(mAdress, mPort);
}


void TagList::setClientName(const QString &aName)
{
    mClientName = aName;
}

void TagList::onError()
{
    QString errorStr = mWebSocket->errorString();
    qDebug() << errorStr;
    switch (mWebSocket->error())
    {
        case QAbstractSocket::ConnectionRefusedError:
            if(!mAdress.isEmpty() && mPort > 1024)
            {
                QTimer::singleShot(1000*60, [this](){
                    qDebug() << "Reconnect..";
                    connectToServer(mAdress, mPort);});
                return;
            }
            break;

        case QAbstractSocket::RemoteHostClosedError:
            if(!mAdress.isEmpty() && mPort > 1024)
            {
                QTimer::singleShot(1000*30, [this](){
                    connectToServer(mAdress, mPort);});
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
    ClientInformation cl(mClientName);
    mWebSocket->sendTextMessage(cl.getInfo());

    connect(mWebSocket, &QWebSocket::binaryMessageReceived, this, &TagList::onBinaryDataRecieved);
   // QByteArray data;
   // toXml(data);
   // mWebSocket->sendBinaryMessage(data);
    if(!mTagSyncTimer && !mFreeRideFlag)
    {
        mTagSyncTimer = new QTimer(this);
        mTagSyncTimer->setInterval(1000);
        connect(mTagSyncTimer, &QTimer::timeout, this, &TagList::syncTags);
        mTagSyncTimer->start();
    }
    mIsConnected = true;
    emit connected();
}


void TagList::onDisconnected()
{
    mIsConnected = false;
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
            if(stream.name() == "create")
            {
                while(!(stream.tokenType() == QXmlStreamReader::EndElement &&
                        stream.name() == "create"))
                {
                    if(stream.readNext() != QXmlStreamReader::StartElement)
                        continue;

                    if(stream.name() == "tag")
                    {
                        createTag(stream);
                    }
                }
            }
            else if(stream.name() == "update")
            {
                while(!(stream.tokenType() == QXmlStreamReader::EndElement &&
                        stream.name() == "update"))
                {
                    if(stream.readNext() != QXmlStreamReader::StartElement)
                        continue;

                    if(stream.name() == "tag")
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

    if(mTagsCreateQueue.size() > 0)
    {
        stream.writeStartElement("create");
        for(int i=0; i<mTagsCreateQueue.size(); ++i)
            mTagsCreateQueue[i]->writeToXml(stream);
        mTagsCreateQueue.clear();
        create = true;
        stream.writeEndElement();
    }
    if(mTagUpdateQueue.size() > 0)
    {
        stream.writeStartElement("update");
        for(int i=0; i<mTagUpdateQueue.size(); ++i)
            mTagUpdateQueue[i]->writeToXml(stream);
        mTagUpdateQueue.clear();
        stream.writeEndElement();
        update = true;
    }

    stream.writeEndElement();
    stream.writeEndDocument();

    if(mWebSocket && (update || create))
    {
        qDebug() << "Sending data ";
        mWebSocket->sendBinaryMessage(data);
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

    if(type == "Double")
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eDouble);
        tag->setValue(attribs.value("value").toDouble());
    }
    else if(type == "Int")
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eInt);
        tag->setValue(attribs.value("value").toInt());
    }
    else if(type == "Bool")
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eBool);
        tag->setValue(attribs.value("value").toInt() == 1 ? true : false);
    }
    else if(type == Tag::toString(Tag::eString))
    {
        tag = TagList::sGetInstance().createTag(subsystem, name, Tag::eString);
        tag->setValue(attribs.value("value").toString());
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
        timestamp = attribs.value("timestamp").toLongLong();

    Tag *tag = findByTagName(QString("%1.%2").arg(subsystem).arg(name));
    if(!tag)
    {
        tag = createTag(subsystem, name, Tag::typeFromString(type));
        qDebug() << "Local tag do not exist, create tag:" << QString("%1.%2").arg(subsystem).arg(name);
    }

    if(tag->getType() == Tag::eDouble)
    {
        double value = attribs.value("value").toDouble();
        tag->setValue(value, timestamp);
        qDebug() << "Update: " << QString("%1.%2").arg(subsystem).arg(name) << value;
    }
    else if(tag->getType() == Tag::eInt)
    {
        int value = attribs.value("value").toInt();
        tag->setValue(value, timestamp);
        qDebug() << "Update: " << QString("%1.%2").arg(subsystem).arg(name) << value;
    }
    else if(tag->getType() == Tag::eBool)
    {
        bool value = attribs.value("value").toInt() == 1 ? true : false;
        tag->setValue(value, timestamp);
        qDebug() << "Update: " << QString("%1.%2").arg(subsystem).arg(name) << value;
    }
    else if(tag->getType() == Tag::eString)
    {
        QString value = attribs.value("value").toString();
        tag->setValue(value, timestamp);
        qDebug() << "Update: " << QString("%1.%2").arg(subsystem).arg(name) << value;
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
    if(!mWebSocket)
        return;

    if(mTagUpdateQueue.contains(aTag))
        mTagUpdateQueue.removeAll(aTag);

    mTagUpdateQueue.push_back(aTag);

    if(mFreeRideFlag)
        syncTags();
}


void TagList::onRecieveDatagrams()
{
    QByteArray datagram;
    while(mUdpSocket->hasPendingDatagrams())
    {
        datagram.resize(int(mUdpSocket->pendingDatagramSize()));
        mUdpSocket->readDatagram(datagram.data(), datagram.size());
    }

    qDebug() << __FUNCTION__ << "datagram: " << datagram;
    if(mIsConnected)
        return;
    if(!mAutoconnectOnBroadcastFlag)
        return;

    QString msg(datagram);
    if(msg.startsWith("juneserveronline"))
    {
        QStringList list = msg.split(":");
        if(list.size() < 1)
            return;
        connectToServer(list[1], 5000);
    }
}


void TagList::setAutoconnectOnBroadcast(bool aAutoconnect)
{
    mAutoconnectOnBroadcastFlag = aAutoconnect;
}
