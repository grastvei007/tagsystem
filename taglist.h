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

#ifndef TAGLIST_H
#define TAGLIST_H

#include "tagsystem_global.h"
#include <QObject>
#include <QMap>
#include <QVector>

#include "tag.h"

class QWebSocket;
class QTimer;

class TAGSYSTEMSHARED_EXPORT TagList : public QObject
{
    Q_OBJECT
public:
    static TagList& sGetInstance();

    int getNumberOfTags() const;

    Tag* createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType);
    Tag* createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType, QVariant initValue);
    Tag* createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType, QVariant initValue, const QString& description);
    Tag* createTag(QXmlStreamReader &aStream);
    Tag* updateTag(QXmlStreamReader &aStream);

    Tag* findByTagName(const QString &aName);
    Tag* getTagByIndex(int aIndex);
    const QString& clientName() const;

    void toXml(QByteArray &rXml, bool aCreate=false) const;

    void freeRide(bool aOn);
    void connectToServer(const QString &aAdress, qint16 aPort);
    bool tryToAutoConnect();
    void setClientName(const QString &aName);
    void reconnect();

signals:
    void tagValueChanged(Tag*);
    void valueChanged();
    void tagCreated();

    void error(QString aError);
    void disconnected();
    void connected();

private slots:
    void onConnected();
    void onDisconnected();
    void onBinaryDataRecieved(QByteArray aMsg);
    void onTagValueChanged(Tag *aTag);

    void syncTags();

    void onError();

private:
    TagList();

private:
    QMap<QString, Tag*> tagByName_;
    QVector<Tag*> tags_;
    QString adress_;
    qint16 port_ = 0;

    QWebSocket *webSocket_ = nullptr;

    bool freeRideFlag_ = false;
    // Tags to sync with server if connected.
    QVector<Tag*> tagsCreateQueue_;
    QVector<Tag*> tagUpdateQueue_;
    QTimer *tagSyncTimer_ = nullptr;

    QString clientName_; ///< the name that identify this client when connected to a server.

    bool isConnected_ = false;
};

#endif // TAGLIST_H
