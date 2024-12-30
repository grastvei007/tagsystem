#ifndef JSON_H
#define JSON_H

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <optional>

namespace util::json
{

std::optional<QJsonArray> byteArrayToJsonArray(const QByteArray &array);
std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &array);

} //end namespace

#endif // JSON_H
