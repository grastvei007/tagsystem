#ifndef CLIENTINFORMATION_H
#define CLIENTINFORMATION_H

#include <QObject>
#include <QString>

class ClientInformation
{
public:
    ClientInformation(const QString &mName);

    QString getInfo() const;
private:
    QString mClientName;
    QString mClientIp;
};

#endif // CLIENTINFORMATION_H
