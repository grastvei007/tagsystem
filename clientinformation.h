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

#ifndef CLIENTINFORMATION_H
#define CLIENTINFORMATION_H

#include "tagsystem_global.h"
#include <QObject>
#include <QString>

class TAGSYSTEMSHARED_EXPORT ClientInformation
{
public:
    ClientInformation(const QString &mName);

    QString getInfo() const;
private:
    QString clientName_;
    QString clientIp_;
};

#endif // CLIENTINFORMATION_H
