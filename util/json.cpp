#include "json.h"

#include <QJsonParseError>
#include <QJsonDocument>

namespace util::json
{

std::optional<QJsonArray> byteArrayToJsonArray(const QByteArray &array)
{
    QJsonParseError error;
    const auto json = QJsonDocument::fromJson(array, &error);
    if (error.error || !json.isArray())
        return std::nullopt;
    return json.array();
}

std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &array)
{
    QJsonParseError error;
    const auto json = QJsonDocument::fromJson(array, &error);
    if (error.error || !json.isObject())
        return std::nullopt;
    return json.object();
}

} //end namespace
