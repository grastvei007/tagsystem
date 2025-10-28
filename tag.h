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

class TagSocket;


class TAGSYSTEMSHARED_EXPORT Tag : public QObject
{
    Q_OBJECT
public:
    enum Type{
        eUnknown,
        eDouble,
        eInt,
        eBool,
        eString,
        eTime
    };
    explicit Tag(QObject *parent = nullptr);
    using EnumMap = std::map<int, QString>;

    Tag(QString subSystem, QString name, Type type);

    Tag(QString subSystem, QString name, Type type, QVariant initValue, const QString &description = {});

    // config
    void setEnumValues(const EnumMap &map); // available if type is eInt

    // setters
    void setValue(QVariant value, qint64 msSinceEpoc = -1);

    // getters
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
    QString enumValue(int value) const;

    // other
    static Type typeFromString(const QString &typeString);
    static Type typeMatchTagSocket(const TagSocket *tagsocket);
    static QString toString(Type type);
    QByteArray toMessage();
    const QJsonObject& toJson();

    bool isUpdated() const {return isUpdated_;}
    void resetUpdateFlag() {isUpdated_ = false;}
    void setUpdatedFlag() {isUpdated_ = true; }
signals:
    void valueChanged(Tag*);
public slots:

private:
    QString subSystem_ = {};
    QString name_ = {};
    Type type_ = Tag::eDouble;
    QString description_ = {};

    // QTime is stored as qint64
    QVariant value_;

    QString timeStampFormat_ = "dd.MM.yyyy hh:mm:ss.zzz";
    qint64 timeStamp_ = QDateTime::currentMSecsSinceEpoch(); ///< msSinceEpoc

    QJsonObject jsonObject_ = QJsonObject();

    bool isUpdated_ = true; ///< local update, indicate ready to be synced with server

    EnumMap enumValues_;
};


#endif // TAG_H
