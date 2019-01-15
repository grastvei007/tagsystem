#ifndef TAGSOCKETLIST_H
#define TAGSOCKETLIST_H

#include <QObject>
#include <QVector>
#include <QMap>

#include "tagsocket.h"


class TagSocketList : public QObject
{
    Q_OBJECT
public:
    static TagSocketList& sGetInstance();

    int getNumberOfTagSockets() const;

    TagSocket* getTagSocketByIndex(int aIndex);

    bool addTagSocket(TagSocket *aTagSocket);
    void removeTagSocket(TagSocket *aTagSocket);

signals:
    void tagSocketAdded();
    void tagSocketRemoved();

public slots:

private:
    QMap<QString, TagSocket*> mTagSocketByName;
    QVector<TagSocket*> mTagSocketList;
};

#endif // TAGSOCKETLIST_H
