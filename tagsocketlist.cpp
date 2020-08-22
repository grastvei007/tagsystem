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

#include <QString>
#include <QDir>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

TagSocketList& TagSocketList::sGetInstance()
{
    static TagSocketList sTagSocketList;
    return sTagSocketList;
}

int TagSocketList::getNumberOfTagSockets() const
{
    return mTagSocketList.size();
}

bool TagSocketList::addTagSocket(TagSocket *aTagSocket)
{
    if(!aTagSocket)
        return false;
    mTagSocketList.push_back(aTagSocket);
    mTagSocketByName[aTagSocket->getFullName()] = aTagSocket;
    connect(aTagSocket, qOverload<TagSocket*>(&TagSocket::valueChanged), this, &TagSocketList::tagSocketValueChanged);
    emit tagSocketAdded();
    return true;
}

void TagSocketList::removeTagSocket(TagSocket *aTagSocket)
{
    mTagSocketByName.remove(aTagSocket->getFullName());
    mTagSocketList.removeAll(aTagSocket);
    emit tagSocketRemoved();
}


TagSocket* TagSocketList::getTagSocketByIndex(int aIndex)
{
    if(aIndex < 0 || aIndex > mTagSocketList.size())
        return nullptr;

    return mTagSocketList.at(aIndex);
}


TagSocket* TagSocketList::getTagSocketByName(QString aName)
{
    if(mTagSocketByName.contains(aName))
    {
        return mTagSocketByName[aName];
    }
    return nullptr;
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
    mApplicationName = aName;
}


void TagSocketList::saveBindingList()
{
#ifdef __linux__
    QString path = QDir::homePath() + QDir::separator() + ".config" + QDir::separator() + "june";
    if(mApplicationName.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Set application name";

    }
    else
        path += QDir::separator() + mApplicationName;
#else
    QString path = qApp->applicationDirPath();
#endif

    QDir dir(path);
    if(!dir.exists())
        QDir().mkpath(path);

    path.append(QDir::separator());
    path.append("tagsocketbindings.xml");
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
        qDebug() << __FUNCTION__ << "Error opening file, " << path;

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("bindings");

    for(int i=0; i<TagSocketList::sGetInstance().getNumberOfTagSockets(); ++i)
    {
        TagSocket *tagsocket = TagSocketList::sGetInstance().getTagSocketByIndex(i);
        stream.writeStartElement("tagsocket");
        stream.writeAttribute("subsystem", tagsocket->getSubSystem());
        stream.writeAttribute("name", tagsocket->getName());
        stream.writeAttribute("type", tagsocket->getTypeStr());
        if(tagsocket->isHookedUp())
        {
            stream.writeAttribute("hookedup", "1");
            Tag *tag = tagsocket->getTag();
            stream.writeAttribute("tagsubsystem", tag->getSubsystem());
            stream.writeAttribute("tagname", tag->getName());
        }
        else
        {
           stream.writeAttribute("hookedup", "0");
        }
        stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndDocument();

    file.close();
}


void TagSocketList::loadBindingList()
{
#ifdef __linux__
    QString path = QDir::homePath() + QDir::separator() + ".config" + QDir::separator() + "june";
    if(mApplicationName.isEmpty())
    {
        qDebug() << __FUNCTION__ << "Set application name";

    }
    else
        path += QDir::separator() + mApplicationName;
#else
  QString path = qApp->applicationDirPath();
#endif
    path.append(QDir::separator());
    path.append("tagsocketbindings.xml");
    QFile file(path);
    if(!file.exists())
    {
        qDebug() << __FUNCTION__ << "File does not exist, " << path;
        return;
    }

    file.open(QIODevice::ReadOnly);
    QXmlStreamReader stream(&file);

    while(!stream.atEnd() && !stream.hasError())
    {
        QXmlStreamReader::TokenType token = stream.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement)
        {
            if(stream.name() == "bindings")
                continue;
            if(stream.name() == "tagsocket")
            {
                QString subsytem = stream.attributes().value("subsystem").toString();
                QString name = stream.attributes().value("name").toString();
                QString type = stream.attributes().value("type").toString();
                TagSocket::Type t;
                if(type == "Bool")
                    t = TagSocket::eBool;
                else if(type == "Int")
                    t = TagSocket::eInt;
                else if(type == "Double")
                    t = TagSocket::eDouble;
                else if(type == "String")
                    t = TagSocket::eString;
                else
                    Q_UNREACHABLE();

                TagSocket *tagsocket = TagSocket::createTagSocket(subsytem, name, t);
                if(stream.attributes().value("hookedup").toInt() == 1)
                {
                    QString tagsubsystem = stream.attributes().value("tagsubsystem").toString();
                    QString tagname = stream.attributes().value("tagname").toString();
                    tagsocket->hookupTag(tagsubsystem, tagname);
                }
            }
        }
    }
    if(stream.hasError())
    {
        qDebug() << __FUNCTION__ << stream.errorString();
    }

    file.close();
}
