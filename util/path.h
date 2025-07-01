#include <QDir>

namespace util::path{

inline QString configDir(const QString &appName)
{
    QString path = QDir::homePath() + QDir::separator() + ".config"
                   + QDir::separator() + "june"
                   + QDir::separator() + appName;

    if(QDir dir(path); !dir.exists())
        QDir().mkpath(path);
    return path;
}

} // end namespace

