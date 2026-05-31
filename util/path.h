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

// Return absolute path for config file in config dir.
// .config/june/<file>
inline QString configFile(const QString &file)
{
    QString path = QDir::homePath() + QDir::separator() + ".config" + QDir::separator() + "june";

    if (QDir dir(path); !dir.exists())
        QDir().mkpath(path);

    path += QDir::separator();
    path += file;

    return path;
}
} // end namespace

