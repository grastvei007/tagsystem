#ifndef TAGVALUE_H
#define TAGVALUE_H

#include <QString>
#include <variant>
#include <vector>

enum class TagType{
	eDouble = 0,
	eInt,
	eBool,
	eString,
	eTime,
	eUnknown
};

template<TagType V>
class TagValue;

template<>
class TagValue<TagType::eDouble>
{
  public:
	using tagType = double;
};

template<>
class TagValue<TagType::eInt>
{
  public:
	using tagType = int;
};

template<>
class TagValue<TagType::eBool>
{
  public:
	using tagType = bool;
};

template<>
class TagValue<TagType::eString>
{
  public:
	using tagType = QString;
};

template<>
class TagValue<TagType::eTime>
{
  public:
	using tagType = qint64;
};

template<TagType... Vs>
using TagValueVariant = std::variant<typename TagValue<Vs>::tagType ...>;

#endif // TAGVALUE_H
