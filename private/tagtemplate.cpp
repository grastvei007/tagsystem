#include "tagtemplate.h"

void Tag2<double>::setValue(double v, qint64 msSinceEpoc)
{
    if(qFuzzyCompare(value_, v))
        return;

    value_ = v;

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    // emit valueChanged(this);
}

void Tag2<int>::setValue(int v, qint64 msSinceEpoc)
{
    if(value_ == v)
        return;

    value_ = v;

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    //emit valueChanged(this);
}

void Tag2<bool>::setValue(bool v, qint64 msSinceEpoc)
{
    if(value_ == v)
        return;
    value_ = v;

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    // emit valueChanged(this);
}

void Tag2<QString>::setValue(QString v, qint64 msSinceEpoc)
{
    if(value_ == v)
        return;

    value_ = v;

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    //  emit valueChanged(this);
}

void Tag2<qint64>::setValue(qint64 v, qint64 msSinceEpoc)
{
    if(value_ == v)
        return;

    value_ = v;

    if(msSinceEpoc < 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
        isUpdated_ = true;
    }
    else
        timeStamp_ = msSinceEpoc;
    //  emit valueChanged(this);
}
