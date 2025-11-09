#ifndef TAG2_H
#define TAG2_H

#include "private/tagtemplate.h"
#include <optional>

namespace tagsystem{

enum class TagType {
    //    eUnknown = 0,
    eDouble = 1,
    eInt,
    eBool,
    eString,
    eTime
};

inline std::optional<TagType> tagTypeFromString(const QString& string)
{
    if(string.compare(QString("Int"), Qt::CaseInsensitive) == 0)
        return TagType::eInt;
    else if(string.compare(QString("Bool"), Qt::CaseInsensitive) == 0)
        return TagType::eBool;
    else if(string.compare(QString("Double"), Qt::CaseInsensitive) == 0)
        return TagType::eDouble;
    else if(string.compare(QString("String"), Qt::CaseInsensitive) == 0)
        return TagType::eString;
    else if(string.compare(QString("Time"), Qt::CaseInsensitive) == 0)
        return TagType::eTime;

    return {};
}

template<TagType V>
class Tag;

template<>
class Tag<TagType::eDouble> : public Tag2<double>
{
public:
    Tag() = default;
    Tag(QString subSystem, QString name)
        : Tag2<double>(subSystem, name)
    {}
    Tag(QString subSystem, QString name, double initValue, const QString &description = {})
        : Tag2<double>(subSystem, name, initValue, description)
    {}

    using tagType = double;

    TagType type() const { return TagType::eDouble; }
};

template <>
class Tag<TagType::eInt> : public Tag2<int>
{
public:
    Tag() = default;
    Tag(QString subSystem, QString name)
        : Tag2<int>(subSystem, name)
    {}
    Tag(QString subSystem, QString name, int initValue, const QString &description = {})
        : Tag2<int>(subSystem, name, initValue, description)
    {}

    using tagType = int;

    TagType type() const { return TagType::eInt; }
};

template <>
class Tag<TagType::eBool> : public Tag2<bool>
{
public:
    Tag() = default;
    Tag(QString subSystem, QString name)
        : Tag2<bool>(subSystem, name)
    {}
    Tag(QString subSystem, QString name, bool initValue, const QString &description = {})
        : Tag2<bool>(subSystem, name, initValue, description)
    {}

    using tagType = bool;

    TagType type() const { return TagType::eBool; }
};

template <>
class Tag<TagType::eString> : public Tag2<QString>
{
public:
    Tag() = default;
    Tag(QString subSystem, QString name)
        : Tag2<QString>(subSystem, name)
    {}
    Tag(QString subSystem, QString name, QString initValue, const QString &description = {})
        : Tag2<QString>(subSystem, name, initValue, description)
    {}

    using tagType = QString;

    TagType type() const { return TagType::eString; }
};

template <>
class Tag<TagType::eTime> : public Tag2<qint64>
{
public:
    Tag() = default;
    Tag(QString subSystem, QString name)
        : Tag2<qint64>(subSystem, name)
    {}
    Tag(QString subSystem, QString name, qint64 initValue, const QString &description = {})
        : Tag2<qint64>(subSystem, name, initValue, description)
    {}

    using tagType = qint64;

    TagType type() const { return TagType::eTime; }
};

//template<TagType... Vs>
//using TagVariant = std::variant<Tag<Vs>...>;

using TagVariant = std::variant<Tag<TagType::eDouble>,
                                Tag<TagType::eInt>,
                                Tag<TagType::eBool>,
                                Tag<TagType::eString>,
                                Tag<TagType::eTime>>;

using Tag2Variant = std::variant<Tag2<double>, Tag2<int>, Tag2<bool>, Tag2<QString>, Tag2<qint64>>;

template<TagType V>
auto makeTag(QString subsystem, QString name)
{
    return TagVariant(Tag<V>(subsystem, name));
}

template<TagType V>
auto makeTag(QString subSystem,
             QString name,
             typename Tag<V>::tagType initValue,
             const QString &description = {})
{
    return TagVariant(Tag<V>(subSystem, name, initValue, description));
}

}// end namespace

#endif // TAG2_H
