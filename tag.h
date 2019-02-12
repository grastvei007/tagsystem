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

#include <QObject>
#include <QString>
#include <QByteArray>

class QXmlStreamReader;
class QXmlStreamWriter;


class Tag : public QObject
{
    Q_OBJECT
public:
    enum Type{
        eDouble,
        eInt,
        eBool,
        eString
    };
    explicit Tag(QObject *parent = nullptr);

    Tag(QString aSubSystem, QString aName, Type aType);

    void setValue(double aValue);
    void setValue(int aValue);
    void setValue(bool aValue);
    void setValue(QString aValue);

    Type getType() const;
    QString getTypeStr() const;
    QString getFullName() const;
    QString getSubsystem() const;
    QString getName() const;

    double getDoubleValue() const;
    int getIntValue() const;
    bool getBoolValue() const;
    QString getStringValue() const;

    void writeToXml(QXmlStreamWriter &aStream);
    static Tag* createFromXml(const QXmlStreamReader &aReader);
    QByteArray toMessage();
signals:
    void valueChanged(Tag*);
public slots:

private:
    QString mSubSystem;
    QString mName;
    Type mType;

    double mDoubleValue;
    int mIntValue;
    bool mBoolValue;
    QString mStringValue;
};


#endif // TAG_H
