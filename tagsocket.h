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

#ifndef TAGSOCKET_H
#define TAGSOCKET_H

#include "tagsystem_global.h"
#include <QObject>
#include <QString>

#include "tag.h"

class TAGSYSTEMSHARED_EXPORT TagSocket : public QObject
{
    Q_OBJECT
public:
    enum Type{
        eDouble,
        eInt,
        eBool,
        eString,
        eTime
    };
    ///< construct a new tagsocket.
    static TagSocket* createTagSocket(QString aSubSystem, QString aName, Type aType);

    ~TagSocket();

    bool hookupTag(Tag *aTag);
    bool hookupTag(QString aTagSubsytem, QString aTagName);
    bool isHookedUp() const;
    bool isWaitingForTag() const;

    QString getFullName() const;
    QString getSubSystem() const;
    QString getName() const;
    QString getTypeStr() const;
    QString getTagName() const;
    Type getType() const;
    Tag* getTag() const;

    void writeValue(double aValue);
    void writeValue(bool aValue);
    void writeValue(int aValue);
    void writeValue(QString aValue);
    void writeValue(QDateTime aValue);

    bool readValue(double &rValue);
    bool readValue(bool &rValue);
    bool readValue(int &rValue);
    bool readValue(QString &rValue);
    bool readValue(QDateTime &rValue);

    static Type typeFromString(const QString &aTypeString);
    static QString toString(const Type type);
signals:
    void valueChanged(double);
    void valueChanged(bool);
    void valueChanged(int);
    void valueChanged(QString);
    void valueChanged(QDateTime);
    void valueChanged(TagSocket*);

private slots:
    void onTagValueChanged(Tag* aTag);
    void onTagCreated();

private:
    TagSocket(QString aSubSystem, QString aName, Type aType);

private:
    Tag *tag_ = nullptr;
    QString tagName_ = QString();

    QString subSystem_;
    QString name_;
    Type type_;
    bool isWaitingForTag_ = false;
};

#endif // TAGSOCKET_H
