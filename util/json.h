#ifndef JSON_H
#define JSON_H

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <optional>
#include <iterator>

namespace util::json
{

std::optional<QJsonArray> byteArrayToJsonArray(const QByteArray &array);
std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &array);


QJsonArray toJsonArray(auto& vector)
{
    return toJsonArray(vector, [](const auto& value){return QJsonValue(value);});
}

template<typename ConvertFunc>
QJsonArray toJsonArray(auto& vector, ConvertFunc toJsonValue)
{
    QJsonArray result;
    std::ranges::transform(vector, std::back_inserter(result), toJsonValue);
    return result;
}

} //end namespace

#endif // JSON_H
