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

#include "tagvalue.h"

class TagSocket;


class TAGSYSTEMSHARED_EXPORT Tag : public QObject
{
    Q_OBJECT
public:

    explicit Tag(QObject *parent = nullptr);
    using EnumMap = std::map<int, QString>;

	Tag(QString subSystem, QString name, TagType type, bool isArray = false);

	Tag(QString subSystem, QString name, TagType type, QVariant initValue, const QString &description = {}, bool isArray = false);

    // config
    void setEnumValues(const EnumMap &map); // available if type is eInt

    // setters
    void setValue(QVariant value, qint64 msSinceEpoc = -1);

	//setters for array
	void insert(unsigned int pos, QVariant value);
	void push_back(QVariant value);
	void syncArray();

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
	bool isArray() const;

    double getDoubleValue() const;
    int getIntValue() const;
    bool getBoolValue() const;
    QString getStringValue() const;
    QDateTime getTimeValue() const;
    QString enumValue(int value) const;
    QString getValueAsString() const;

	// getters for array
	unsigned int size() const;


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
  bool isEqual(const QVariantList &list) const;
    QString subSystem_ = {};
    QString name_ = {};
    TagType type_ = TagType::eDouble;
    QString description_ = {};

    // QTime is stored as qint64
	using tagValue_t = TagValueVariant<TagType::eDouble, TagType::eInt, TagType::eBool, TagType::eString, TagType::eTime>;
	std::vector<tagValue_t> tagValue_;
	bool isArray_ = false;

    QString timeStampFormat_ = "dd.MM.yyyy hh:mm:ss.zzz";
    qint64 timeStamp_ = QDateTime::currentMSecsSinceEpoch(); ///< msSinceEpoc

    QJsonObject jsonObject_ = QJsonObject();

    bool isUpdated_ = true; ///< local update, indicate ready to be synced with server

    EnumMap enumValues_;

	void updateJsonObject();
};


#endif // TAG_H
