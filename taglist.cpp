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
#include <QTimer>
#include <QDebug>

#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>

#include "clientinformation.h"
#include "util/json.h"

TagList& TagList::sGetInstance()
{
    static TagList sTagList;
    return sTagList;
}

int TagList::getNumberOfTags() const
{
    return tagByName_.size();
}

Tag* TagList::createTag(const QString &subSystem, const QString &name, TagType type)
{
    Tag *tag = findByTagName(QString("%1.%2").arg(subSystem, name));
    if(tag)
    {
        return tag;
    }
    // tag does not exist, create it.
    tag = new Tag(subSystem, name, type);
    tagByName_[tag->getFullName()] = tag;
    tags_.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);
    subsystems_.push_back(subSystem);
    qDebug() << "Create tag: " << tag->getFullName() << " (" << tags_.size() << ")";
    emit tagCreated(tags_.count());
    emit tagValueChanged(tag);
    return tag;
}


Tag *TagList::createTag(const QString &subSystem, const QString &name, TagType type, QVariant initValue)
{
    return createTag(subSystem, name, type, initValue, QString());
}


Tag *TagList::createTag(const QString &subSystem, const QString &name, TagType type, QVariant initValue, const QString &description)
{
    auto *tag = findByTagName(QString("%1.%2").arg(subSystem, name));
    if(tag)
        return tag;

	tag = new Tag(subSystem, name, type, initValue, description, isArray);

    tagByName_[tag->getFullName()] = tag;
    tags_.push_back(tag);
    connect(tag, &Tag::valueChanged, this, &TagList::tagValueChanged);
    connect(tag, &Tag::valueChanged, this, &TagList::onTagValueChanged);

    subsystems_.push_back(subSystem);

    qDebug() << "Create tag: " << tag->getFullName() << " (" << tags_.size() << ")";
    emit tagCreated(tags_.count());
    emit tagValueChanged(tag);
    return tag;
}


Tag* TagList::findByTagName(const QString &fullname)
{
    if(!tagByName_.contains(fullname))
        return nullptr;

    return tagByName_[fullname];
}

Tag *TagList::findByTagName(const QString &subsystem, const QString &name)
{
    return findByTagName(QString("%1.%2").arg(subsystem, name));
}

std::vector<Tag *> TagList::findTagsInSubsystem(const QString &subsystem) const
{
    std::vector<Tag *> result;

    for (Tag *tag : tags_)
    {
        if (tag->getSubsystem() == subsystem)
            result.push_back(tag);
    }

    return result;
}

Tag* TagList::getTagByIndex(int index)
{
    return tags_.at(index);
}

const QStringList &TagList::subsystems()
{
    subsystems_.removeDuplicates();
    return subsystems_;
}

const QString &TagList::clientName() const
{
    return clientName_;
}

const QString &TagList::adress() const
{
    return adress_;
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

void TagList::connectToServer(const QString &adress, qint16 port)
{
    if(clientName_.isEmpty())
        qFatal("Set client name before connecting to server..");
    adress_ = adress;
    port_ = port;
    QUrl url(QString("ws://%1:%2").arg(adress, QString::number(port)));
    qDebug() << "Connect to: " << url;
	if (!webSocket_)
	{
		webSocket_ = new QWebSocket;
		connect(webSocket_, &QWebSocket::connected, this, &TagList::onConnected);
		connect(webSocket_, &QWebSocket::disconnected, this, &TagList::onDisconnected);
		connect(webSocket_,
				static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(
					&QWebSocket::errorOccurred),
				this,
				&TagList::onError);
	}
	webSocket_->open(url);
}

void TagList::disconnectFromServer()
{
    if(!webSocket_)
        return;
	isConnected_ = false; // make flag ready before to close the connection.
	webSocket_->disconnect();
	webSocket_->deleteLater();
	webSocket_ = nullptr;
	emit serverDisconnected();
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

void TagList::setReconnectIfServerCloseConnection()
{
	shouldReconnecIfConnectionIsClosed_ = true;
}

void TagList::setClientName(const QString &name)
{
    clientName_ = name;
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
                QTimer::singleShot(1000*60, this, [this](){
                    qDebug() << "Reconnect..";
                    connectToServer(adress_, port_);});
                return;
            }
            break;

        case QAbstractSocket::RemoteHostClosedError:
            if(!adress_.isEmpty() && port_ > 1024)
            {
                QTimer::singleShot(1000*30, this, [this](){
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
	if (isConnected_ && shouldReconnecIfConnectionIsClosed_)
	{
		isConnected_ = false;
		if (!adress_.isEmpty() && port_ > 0)
		{
			connectToServer(adress_, port_);
		}
	}
	else
	{
		isConnected_ = false;
	}
	emit serverDisconnected();
}


/**
 * @brief TagList::onBinaryDataRecieved
 * @param aMsg
 *
 * Recieve list of tags to create or update.
 */
void TagList::onBinaryDataRecieved(QByteArray msg)
{
    auto document = QJsonDocument::fromJson(msg);
    const auto array = document.array();

    for (const auto &jsonRef : array)
    {
        const auto &jsonTag = jsonRef.toObject();
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
void TagList::onTagValueChanged(Tag *tag)
{
    int index = tags_.indexOf(tag);
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
    case TagType::eDouble: {
        if (tag)
            tag->setValue(value.toDouble(), timestamp);
        else
            createdTag = createTag(subsystem, name, TagType::eDouble, value.toDouble(), description);
        break;
    }
    case TagType::eInt: {
        Tag::EnumMap list;
        if(json.contains("enumvalues"))
        {
            const auto array = json.value("enumvalues").toArray();
            auto transform = [](const QJsonValue &value)
            {
                const QJsonObject &obj = value.toObject();
                int key = obj.value("key").toInt();
                QString str = obj.value("value").toString();
                return std::pair<int, QString>{key, str};
            };

            for(const auto &elm : array)
            {
                list.insert(transform(elm));
            }
        }
        if (tag)
        {
            tag->setValue(value.toInt(), timestamp);
            if(!list.empty())
                tag->setEnumValues(list);
        }
        else
        {
            createdTag = createTag(subsystem, name, TagType::eInt, value.toInt(), description);
            if(!list.empty())
                createdTag->setEnumValues(list);
        }

        break;
    }
    case TagType::eBool: {
        if (tag)
            tag->setValue(value.toBool(), timestamp);
        else
            createdTag = createTag(subsystem, name, TagType::eBool, value.toBool(), description);
        break;
    }
    case TagType::eString: {
        if (tag)
            tag->setValue(value.toString(), timestamp);
        else
            createdTag = createTag(subsystem, name, TagType::eString, value.toString(), description);
        break;
    }
    case TagType::eTime: {
        qint64 time = value.toInteger();
        if (tag)
            tag->setValue(time, timestamp);
        else
            createdTag = createTag(subsystem, name, TagType::eTime, time, description);
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

void TagList::clear()
{
    tagByName_.clear();
    for(auto *tag : tags_)
        tag->deleteLater();
    tags_.clear();

    isConnected_ = false;
    initialTagBurstReceived_ = false;
    subsystems_.clear();
}
