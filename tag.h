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
#include <QJsonObject>

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

    Tag(QString subSystem, QString name, Type type);

    Tag(QString subSystem, QString name, Type type, double initValue, const QString &description = QString());
    Tag(QString subSystem, QString name, Type type, int initValue, const QString &description = QString());
    Tag(QString subSystem, QString name, Type type, bool initValue, const QString &description = QString());
    Tag(QString subSystem, QString name, Type type, QString initValue, const QString &description = QString());
    Tag(QString subSystem, QString name, Type type, QDateTime initValue, const QString &description = QString());

    void setValue(double value, qint64 msSinceEpoc=-1);
    void setValue(int value, qint64 msSinceEpoc=-1);
    void setValue(bool value, qint64 msSinceEpoc=-1);
    void setValue(QString value, qint64 msSinceEpoc=-1);
    void setValue(QDateTime value, qint64 msSinceEpoc=-1);

    Type getType() const;
    QString getTypeStr() const;
    QString getFullName() const;
    QString getSubsystem() const;
    QString getName() const;
    QString getTimeStamp() const;
    const QString& getDescription() const;
    const QString& getTimeStampFormat() const;
    qint64 getMsSinceEpoc() const;

    double getDoubleValue() const;
    int getIntValue() const;
    bool getBoolValue() const;
    QString getStringValue() const;
    QDateTime getTimeValue() const;


    void writeToXml(QXmlStreamWriter &stream);
    static Tag* createFromXml(const QXmlStreamReader &reader);
    static Type typeFromString(const QString &typeString);
    static QString toString(Type type);
    QByteArray toMessage();
    const QJsonObject& toJson();
signals:
    void valueChanged(Tag*);
public slots:

private:
    QString subSystem_ = QString();
    QString name_ = QString();
    Type type_ = Tag::eDouble;
    QString description_ = QString();

    double doubleValue_ = 0.0;
    int intValue_ = 0;
    bool boolValue_ = false;
    QString stringValue_ = QString();

    qint64 timeValue_ = 0; ///< value for tag type time
    QString timeStampFormat_ = "dd.MM.yyyy hh:mm:ss.zzz";
    qint64 timeStamp_ = QDateTime::currentMSecsSinceEpoch(); ///< msSinceEpoc

    QJsonObject jsonObject_ = QJsonObject();
};


#endif // TAG_H
