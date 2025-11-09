#ifndef TAGTEMPLATE_H
#define TAGTEMPLATE_H

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <map>
#include <memory>

class QTag2Base : public QObject
{
    Q_OBJECT
public:
    QTag2Base() = default;
signals:
    void valueChanged(QTag2Base*);
};

template<typename T>
class Tag2Base // : public QTag2Base
{
public:
    using EnumMap = std::map<int, QString>;

    Tag2Base() = default;
    Tag2Base(QString subSystem, QString name)
        : subsystem_(subSystem)
        , name_(name)
    //  , base_(new QTag2Base)
    {}
    Tag2Base(QString subSystem, QString name, T initValue, const QString &description)
        : subsystem_(subSystem)
        , name_(name)
        , value_(initValue)
        , description_(description)
    // , base_(new QTag2Base)
    {}

    //config
    void setEnumValues(const EnumMap &map) { enumValues_ = map; }

    // getters
    T value() const {return value_;}
    const QString& subSystem() const { return subsystem_;}
    const QString& name() const { return name_;}
    QString fullName() const {return QString("%1.%2").arg(subsystem_, name_);}
    const QString& description() const {return description_;}
    QString timeStamp() const {return QDateTime::fromMSecsSinceEpoch(timeStamp_).toString(timeStampFormat_);}
    const QString& timeStampFormat() const {return timeStampFormat_;}
    qint64 msSinceEpoc() const { return timeStamp_;}
    QString enumValue(int value) const
    {
        if(enumValues_.count(value))
            return enumValues_.at(value);
        return {};
    }

    bool isUpdated() const {return isUpdated_;}
    void resetUpdateFlag() {isUpdated_ = false;}
    void setUpdatedFlag() {isUpdated_ = true;}
protected:
    T value_;
    QString subsystem_ = {};
    QString name_ = {};
    QString description_ = {};

    QString timeStampFormat_ = "dd.MM.yyyy hh:mm:ss.zzz";
    qint64 timeStamp_ = QDateTime::currentMSecsSinceEpoch(); ///< msSinceEpoc

    bool isUpdated_ = true; ///< local update, indicate ready to be synced with server
    EnumMap enumValues_;
    //std::unique_ptr<QTag2Base> base_;
};

template <typename T>
class Tag2 : public Tag2Base<T>
{
public:
    Tag2() = default;
    Tag2(QString subSystem, QString name);
};

template<>
class Tag2<double> : public Tag2Base<double>
{
public:
    Tag2() = default;
    Tag2(QString subSystem, QString name)
        : Tag2Base<double>(subSystem, name)
    {}
    Tag2(QString subSystem, QString name, double initValue, const QString &description)
        : Tag2Base<double>(subSystem, name, initValue, description)
    {}

    void setValue(double v, qint64 msSinceEpoc = -1);

    QJsonObject jsonObject_ = QJsonObject();

    const QJsonObject &toJson()
    {
        jsonObject_.insert("name", name());
        jsonObject_.insert("subsystem", subSystem());
        jsonObject_.insert("type", "Double");
        jsonObject_.insert("description", description());
        jsonObject_.insert("timestamp", timeStamp_);
        jsonObject_.insert("value", value());

        return jsonObject_;
    }
};

template<>
class Tag2<int> : public Tag2Base<int>
{
public:
    Tag2() = default;
    Tag2(QString subSystem, QString name)
        : Tag2Base<int>(subSystem, name)
    {}
    Tag2(QString subSystem, QString name, int initValue, const QString &description)
        : Tag2Base<int>(subSystem, name, initValue, description)
    {}

    void setValue(int v, qint64 msSinceEpoc = -1);

    using type = int;
    QJsonObject jsonObject_ = QJsonObject();

    const QJsonObject &toJson()
    {
        jsonObject_.insert("name", name());
        jsonObject_.insert("subsystem", subSystem());
        jsonObject_.insert("type", "Int");
        jsonObject_.insert("description", description());
        jsonObject_.insert("timestamp", timeStamp_);
        jsonObject_.insert("value", value());

        return jsonObject_;
    }
};

template<>
class Tag2<bool> : public Tag2Base<bool>
{
public:
    Tag2() = default;
    Tag2(QString subSystem, QString name)
        : Tag2Base<bool>(subSystem, name)
    {}
    Tag2(QString subSystem, QString name, bool initValue, const QString &description)
        : Tag2Base<bool>(subSystem, name, initValue, description)
    {}

    void setValue(bool v, qint64 msSinceEpoc = -1);

    using type = bool;
    QJsonObject jsonObject_ = QJsonObject();

    const QJsonObject &toJson()
    {
        jsonObject_.insert("name", name());
        jsonObject_.insert("subsystem", subSystem());
        jsonObject_.insert("type", "Bool");
        jsonObject_.insert("description", description());
        jsonObject_.insert("timestamp", timeStamp_);
        jsonObject_.insert("value", value());

        return jsonObject_;
    }
};

template<>
class Tag2<QString> : public Tag2Base<QString>
{
public:
    Tag2() = default;
    Tag2(QString subSystem, QString name)
        : Tag2Base<QString>(subSystem, name)
    {}
    Tag2(QString subSystem, QString name, QString initValue, const QString &description)
        : Tag2Base<QString>(subSystem, name, initValue, description)
    {}

    void setValue(QString v, qint64 msSinceEpoc = -1);

    using type = bool;
    QJsonObject jsonObject_ = QJsonObject();

    const QJsonObject &toJson()
    {
        jsonObject_.insert("name", name());
        jsonObject_.insert("subsystem", subSystem());
        jsonObject_.insert("type", "String");
        jsonObject_.insert("description", description());
        jsonObject_.insert("timestamp", timeStamp_);
        jsonObject_.insert("value", value());

        return jsonObject_;
    }
};

template<>
class Tag2<qint64> : public Tag2Base<qint64>
{
public:
    Tag2() = default;
    Tag2(QString subSystem, QString name)
        : Tag2Base<qint64>(subSystem, name)
    {}
    Tag2(QString subSystem, QString name, qint64 initValue, const QString &description)
        : Tag2Base<qint64>(subSystem, name, initValue, description)
    {}

    void setValue(qint64 v, qint64 msSinceEpoc = -1);

    using type = qint64;
    QJsonObject jsonObject_ = QJsonObject();

    const QJsonObject &toJson()
    {
        jsonObject_.insert("name", name());
        jsonObject_.insert("subsystem", subSystem());
        jsonObject_.insert("type", "Time");
        jsonObject_.insert("description", description());
        jsonObject_.insert("timestamp", timeStamp_);
        jsonObject_.insert("value", value());

        return jsonObject_;
    }
};

#endif // TAGTEMPLATE_H
