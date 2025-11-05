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

enum class TagType{
    eUnknown = 0,
    eDouble,
    eInt,
    eBool,
    eString,
    eTime
};

template<TagType V>
struct TagTypeType;

template<>
struct TagTypeType<TagType::eUnknown>
{
    using type = bool;
};

template<>
struct TagTypeType<TagType::eInt>
{
    using type = int;
};

template<>
struct TagTypeType<TagType::eDouble>
{
    using type = double;
};

template<>
struct TagTypeType<TagType::eBool>
{
    using type = bool;
};

template<>
struct TagTypeType<TagType::eString>
{
    using type = QString;
};

template<>
struct TagTypeType<TagType::eTime>
{
    using type = qint64;
};

template<TagType... Vs>
using TagTypeVariant = std::variant<typename TagTypeType<Vs>::type...>;


class TAGSYSTEMSHARED_EXPORT Tag : public QObject
{
    Q_OBJECT
public:

    explicit Tag(QObject *parent = nullptr);
    using EnumMap = std::map<int, QString>;

    Tag(QString subSystem, QString name, TagType type);

    Tag(QString subSystem, QString name, TagType type, QVariant initValue, const QString &description = {});

    // config
    void setEnumValues(const EnumMap &map); // available if type is eInt

    // setters
    void setValue(QVariant value, qint64 msSinceEpoc = -1);

    // getters
    TagType getType() const;
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
    static TagType typeFromString(const QString &typeString);
    static TagType typeMatchTagSocket(const TagSocket *tagsocket);
    static QString toString(TagType type);
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
    TagType type_ = TagType::eUnknown;
    QString description_ = {};

    // QTime is stored as qint64
    TagTypeVariant<TagType::eUnknown, TagType::eDouble, TagType::eInt, TagType::eBool, TagType::eString, TagType::eTime> value_;

    QString timeStampFormat_ = "dd.MM.yyyy hh:mm:ss.zzz";
    qint64 timeStamp_ = QDateTime::currentMSecsSinceEpoch(); ///< msSinceEpoc

    QJsonObject jsonObject_ = QJsonObject();

    bool isUpdated_ = true; ///< local update, indicate ready to be synced with server

    EnumMap enumValues_;
};


#endif // TAG_H
