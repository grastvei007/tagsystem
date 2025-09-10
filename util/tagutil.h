#ifndef TAGUTIL_H
#define TAGUTIL_H

#include <utility>
#include <QString>
#include <QList>

namespace util::tag
{
inline std::pair<QString, QString> splitFullName(const QString &fullName)
{
    auto list = fullName.split(".");
    return std::pair<QString, QString>(list.first(), list.last());
}

inline QString fullName(const QString& subsystem, const QString& name)
{
    return QString("%1.%2").arg(subsystem, name);
}

} // end namespace


#endif // TAGUTIL_H
