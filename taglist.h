#ifndef TAGLIST_H
#define TAGLIST_H

#include <QObject>
#include <QMap>
#include <QVector>

#include "tag.h"

class QWebSocket;

class QTimer;

class TagList : public QObject
{
    Q_OBJECT
public:
    static TagList& sGetInstance();

    int getNumberOfTags() const;

    Tag* createTag(const QString &aSubSystem, const QString &aName, Tag::Type aType);
    Tag* createTag(QXmlStreamReader &aStream);
    Tag* updateTag(QXmlStreamReader &aStream);

    Tag* findByTagName(const QString &aName);
    Tag* getTagByIndex(int aIndex);

    void toXml(QByteArray &rXml, bool aCreate=false) const;

    void freeRide(bool aOn);
    void connectToServer(const QString &aAdress, qint16 aPort);
    void setClientName(const QString &aName);
signals:
    void valueChanged();
    void tagCreated();

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
    QMap<QString, Tag*> mTagByName;
    QVector<Tag*> mTags;

    QWebSocket *mWebSocket;

    bool mFreeRideFlag;
    // Tags to sync with server if connected.
    QVector<Tag*> mTagsCreateQueue;
    QVector<Tag*> mTagUpdateQueue;
    QTimer *mTagSyncTimer;

    QString mClientName; ///< the name that identify this client when connected to a server.
};

#endif // TAGLIST_H
