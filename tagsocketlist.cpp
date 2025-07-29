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

#include "tagsocketlist.h"
#include "tagsocket.h"

#include <QString>
#include <QDir>
#include <QTimer>
#include <QDebug>

#include <QJsonArray>
#include <QJsonObject>

#ifndef __linux__
    #include <QCoreApplication>
#endif

TagSocketList& TagSocketList::sGetInstance()
{
    static TagSocketList sTagSocketList;
    return sTagSocketList;
}

int TagSocketList::getNumberOfTagSockets() const
{
    return tagSocketList_.size();
}

bool TagSocketList::addTagSocket(TagSocket *aTagSocket)
{
    if(!aTagSocket)
        return false;
    tagSocketList_.push_back(aTagSocket);
    tagSocketByName_[aTagSocket->getFullName()] = aTagSocket;
    connect(aTagSocket, qOverload<TagSocket*>(&TagSocket::valueChanged), this, &TagSocketList::tagSocketValueChanged);

    if(autoSave_)
        onTagSocketAdded();

    emit tagSocketAdded();
    return true;
}

void TagSocketList::removeTagSocket(TagSocket *aTagSocket)
{
    tagSocketByName_.remove(aTagSocket->getFullName());
    tagSocketList_.removeAll(aTagSocket);
    emit tagSocketRemoved();
}


TagSocket* TagSocketList::getTagSocketByIndex(int aIndex)
{
    if(aIndex < 0 || aIndex > tagSocketList_.size())
        return nullptr;

    return tagSocketList_.at(aIndex);
}

TagSocket *TagSocketList::getTagSocketByName(const QString &aName) const
{
    if(tagSocketByName_.contains(aName))
    {
        return tagSocketByName_[aName];
    }
    return nullptr;
}

TagSocket *TagSocketList::findTagSocketByName(const QString &subsystem, const QString &name) const
{
    const auto fullname = QString("%1.%2").arg(subsystem, name);
    return getTagSocketByName(fullname);
}

/**
 * @brief TagSocketList::setApplicationName
 * @param aName
 *
 * Set the application name.
 * The name is used in the path for save and load tagsocket
 * binding list.
 */
void TagSocketList::setApplicationName(QString aName)
{
    applicationName_ = aName;
}

void TagSocketList::setAutoSave()
{
    autoSave_ = true;
}

QJsonArray TagSocketList::toJsonArray() const
{
    QJsonArray tagsocketArray;

    for(const auto &tagsocket : tagSocketList_)
    {
        tagsocketArray.push_back(tagsocket->toJson());
    }
    return tagsocketArray;
}

void TagSocketList::saveBindingList()
{
#ifdef __linux__
    QString path = QDir::homePath() + QDir::separator() + ".config" + QDir::separator() + "june";
    if(applicationName_.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Set application name";

    }
    else
        path += QDir::separator() + applicationName_;
#else
    QString path = qApp->applicationDirPath();
#endif

    QDir dir(path);
    if(!dir.exists())
        QDir().mkpath(path);

    path.append(QDir::separator());
    path.append("tagsocketbindings.json");
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << __FUNCTION__ << "Error opening file, " << path;
        return;
    }

    QJsonArray tagsocketArray = toJsonArray();

    QJsonObject obj;
    obj.insert("tagsocketbinding", tagsocketArray);
    QJsonDocument document(obj);

    QTextStream stream(&file);
    stream << document.toJson();
    file.close();
}


void TagSocketList::loadBindingList()
{
#ifdef __linux__
    QString path = QDir::homePath() + QDir::separator() + ".config" + QDir::separator() + "june";
    if(applicationName_.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Set application name";

    }
    else
        path += QDir::separator() + applicationName_;
#else
  QString path = qApp->applicationDirPath();
#endif
    path.append(QDir::separator());
    path.append("tagsocketbindings.json");
    QFile file(path);
    if(!file.exists())
    {
        qDebug() << __FUNCTION__ << "File does not exist, " << path;
        return;
    }

    file.open(QIODevice::ReadOnly);

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());

    if(document.isObject())
    {
        QJsonObject object = document.object();

        const QJsonArray tagsockets = object.value("tagsocketbinding").toArray();
        for(const auto &tagsocketRef : tagsockets)
        {
            const QJsonObject &tagsocket = tagsocketRef.toObject();
            TagSocket::createFromJson(tagsocket);
        }
        qDebug() << "TagSockets loaded, " << tagsockets.size();
    }

    file.close();
}

void TagSocketList::onTagSocketAdded()
{
    if(temporaryBlockSave_)
        return;

    temporaryBlockSave_ = true;
    // When called save 10 sec later, before opening for another save.
    // prevent saving many times if an object is created and it has many new tagsockets.
    QTimer::singleShot(10000, this, [this](){
        saveBindingList();
        temporaryBlockSave_ = false;
    });
}
