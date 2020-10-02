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

#ifndef TAG_H
#define TAG_H

#include "tagsystem_global.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>

class QXmlStreamReader;
class QXmlStreamWriter;


class TAGSYSTEMSHARED_EXPORT Tag : public QObject
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
    explicit Tag(QObject *parent = nullptr);

    Tag(QString aSubSystem, QString aName, Type aType);

    void setValue(double aValue, qint64 msSinceEpoc=-1);
    void setValue(int aValue, qint64 msSinceEpoc=-1);
    void setValue(bool aValue, qint64 msSinceEpoc=-1);
    void setValue(QString aValue, qint64 msSinceEpoc=-1);
    void setValue(QDateTime aValue, qint64 msSinceEpoc=-1);

    Type getType() const;
    QString getTypeStr() const;
    QString getFullName() const;
    QString getSubsystem() const;
    QString getName() const;
    QString getTimeStamp() const;
    const QString& getTimeStampFormat() const;
    qint64 getMsSinceEpoc() const;

    double getDoubleValue() const;
    int getIntValue() const;
    bool getBoolValue() const;
    QString getStringValue() const;
    QDateTime getTimeValue() const;


    void writeToXml(QXmlStreamWriter &aStream);
    static Tag* createFromXml(const QXmlStreamReader &aReader);
    static Type typeFromString(const QString &aTypeString);
    static QString toString(Type aType);
    QByteArray toMessage();
signals:
    void valueChanged(Tag*);
public slots:

private:
    QString mSubSystem;
    QString mName;
    Type mType;
    qint64 mTimeStamp; ///< msSinceEpoc

    double mDoubleValue;
    int mIntValue;
    bool mBoolValue;
    qint64 mTimeValue = 0; ///< value for tag type time
    QString mStringValue;
    QString mTimeStampFormat;
};


#endif // TAG_H
