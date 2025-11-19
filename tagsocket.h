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

class QJsonObject;

class TAGSYSTEMSHARED_EXPORT TagSocket : public QObject
{
    Q_OBJECT
public:
    enum Type{
        eNone,
        eDouble,
        eInt,
        eBool,
        eString,
        eTime
    };
    ///< construct a new tagsocket.
    static TagSocket* createTagSocket(QString subSystem, QString name, Type type);
    static TagSocket* createFromJson(const QJsonObject &json);


    ~TagSocket();

    bool hookupTag(Tag *tag);
    bool hookupTag(QString tagSubsytem, QString tagName);
    bool disconnectTag();
    bool isHookedUp() const;
    bool isWaitingForTag() const;

    QString getFullName() const;
    QString getSubSystem() const;
    QString getName() const;
    QString getTypeStr() const;
    QString getTagName() const;
    Type getType() const;
    Tag* getTag() const;

    void setScaleValue(double scale); //< only for double socket

    void writeValue(double value);
    void writeValue(bool value);
    void writeValue(int value);
    void writeValue(QString value);
    void writeValue(QDateTime value);

    bool readValue(double &rValue);
    bool readValue(bool &rValue);
    bool readValue(int &rValue);
    bool readValue(QString &rValue);
    bool readValue(QDateTime &rValue);

    QJsonObject toJson() const;

    static Type typeFromString(const QString &typeString);
    static QString toString(const Type type);
    static Type typeMatchingTag(const Tag *tag);
signals:
    void valueChanged(double);
    void valueChanged(bool);
    void valueChanged(int);
    void valueChanged(QString);
    void valueChanged(QDateTime);
    void valueChanged(TagSocket*);
    void hookedUp();

private slots:
    void onTagValueChanged(Tag* tag);
    void onTagCreated(int index);

private:
    TagSocket(QString aSubSystem, QString name, Type type);

private:
    Tag *tag_ = nullptr;
    QString tagName_ = QString();

    QString subSystem_;
    QString name_;
    Type type_;
    bool isWaitingForTag_ = false;
    double scaleValue_ = 1.0;
};

#endif // TAGSOCKET_H
