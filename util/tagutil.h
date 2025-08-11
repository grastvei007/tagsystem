#ifndef TAGUTIL_H
#define TAGUTIL_H

#include <utility>
#include <QString>

namespace util::tag
{
std::pair<QString, QString> splitFullName(const QString &fullName)
{
    QString subsystem = fullName.split(".").first();
    QString name = fullName.split(".").last();
    return std::pair<QString, QString>(subsystem, name);
}


} // end namespace


#endif // TAGUTIL_H
