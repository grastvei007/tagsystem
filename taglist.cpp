#include "taglist.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QWebSocket>
#include <QTimer>

TagList& TagList::sGetInstance()
{
    static TagList sTagList;
    return sTagList;
}

TagList::TagList() :
    mWebSocket(nullptr),
    mTagSyncTimer(nullptr),
    mFreeRideFlag(false)
{

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
    QUrl url(QString("ws://%1:%2").arg(aAdress).arg(QString::number(aPort)));
    qDebug() << "Connect to: " << url;
    mWebSocket = new QWebSocket;
    connect(mWebSocket, &QWebSocket::connected, this, &TagList::onConnected);
    connect(mWebSocket, &QWebSocket::disconnected, this, &TagList::onDisconnected);
    connect(mWebSocket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &TagList::onError);
    mWebSocket->open(url);
}


void TagList::onError()
{
    qDebug() << mWebSocket->errorString();
    switch (mWebSocket->error())
    {
        case QAbstractSocket::ConnectionRefusedError:
        {

        }
    }

    /*QAbstractSocket::RemoteHostClosedError
    QAbstractSocket::HostNotFoundError
    QAbstractSocket::SocketAccessError
    QAbstractSocket::SocketResourceError
    QAbstractSocket::SocketTimeoutError
    QAbstractSocket::DatagramTooLargeError
    QAbstractSocket::NetworkError
    QAbstractSocket::AddressInUseError
    QAbstractSocket::SocketAddressNotAvailableError
    QAbstractSocket::UnsupportedSocketOperationError
    QAbstractSocket::ProxyAuthenticationRequiredError
    QAbstractSocket::SslHandshakeFailedError
    QAbstractSocket::UnfinishedSocketOperationError
    QAbstractSocket::ProxyConnectionRefusedError
    QAbstractSocket::ProxyConnectionClosedError
    QAbstractSocket::ProxyConnectionTimeoutError
    QAbstractSocket::ProxyNotFoundError
    QAbstractSocket::ProxyProtocolError
    QAbstractSocket::OperationError
    QAbstractSocket::SslInternalError
    QAbstractSocket::SslInvalidUserDataError
    QAbstractSocket::TemporaryError
    QAbstractSocket::UnknownSocketError*/
}


void TagList::onConnected()
{
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
}


void TagList::onDisconnected()
{

}


/**
 * @brief TagList::onBinaryDataRecieved
 * @param aMsg
 *
 * Recieve list of tags to create or update.
 */
void TagList::onBinaryDataRecieved(QByteArray aMsg)
{
    qDebug() << __FUNCTION__ << aMsg;
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

    return tag;
}


Tag* TagList::updateTag(QXmlStreamReader &aStream)
{
    QXmlStreamAttributes attribs = aStream.attributes();
    QString subsystem = attribs.value("subsystem").toString();
    QString name = attribs.value("name").toString();
    QString type = attribs.value("type").toString();

    Tag *tag = findByTagName(QString("%1.%2").arg(subsystem).arg(name));
    if(!tag)
        return nullptr;

    if(tag->getType() == Tag::eDouble)
    {
        tag->setValue(attribs.value("value").toDouble());
    }
    else if(tag->getType() == Tag::eInt)
    {
        tag->setValue(attribs.value("value").toInt());
    }
    else if(tag->getType() == Tag::eBool)
    {
        tag->setValue(attribs.value("value").toInt() == 1 ? true : false);
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
