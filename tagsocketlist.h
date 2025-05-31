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

#ifndef TAGSOCKETLIST_H
#define TAGSOCKETLIST_H

#include "tagsystem_global.h"
#include <QObject>
#include <QVector>
#include <QMap>

class TagSocket;


class TAGSYSTEMSHARED_EXPORT TagSocketList : public QObject
{
    Q_OBJECT
public:
    static TagSocketList& sGetInstance();

    int getNumberOfTagSockets() const;

    TagSocket* getTagSocketByIndex(int aIndex);
    TagSocket* getTagSocketByName(const QString& aName) const;
    TagSocket* findTagSocketByName(const QString& subsystem, const QString& name) const;

    bool addTagSocket(TagSocket *aTagSocket);
    void removeTagSocket(TagSocket *aTagSocket);

    void saveBindingList();
    void loadBindingList();

    void setApplicationName(QString aName); ///< set the appplication name, used in path for config files.
    void setAutoSave();

signals:
    void tagSocketAdded();
    void tagSocketRemoved();
    void tagSocketValueChanged();

public slots:

private:
    void onTagSocketAdded();

    QMap<QString, TagSocket*> tagSocketByName_;
    QVector<TagSocket*> tagSocketList_;

    QString applicationName_;
    bool autoSave_ = false;
    bool temporaryBlockSave_ = false;
};

#endif // TAGSOCKETLIST_H
