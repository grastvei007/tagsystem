#ifndef TAGSOCKET_H
#define TAGSOCKET_H

#include <QObject>
#include <QString>

#include "tag.h"

class TagSocket : public QObject
{
    Q_OBJECT
public:
    enum Type{
        eDouble,
        eInt,
        eBool
    };
    TagSocket(QString aSubSystem, QString aName, Type aType);
    ~TagSocket();

    bool hookupTag(Tag *aTag);
    bool hookupTag(QString aTagSubsytem, QString aTagName);
    bool isHookedUp() const;

    QString getFullName() const;
    QString getSubSystem() const;
    QString getName() const;
    QString getTypeStr() const;
    Type getType() const;
    Tag* getTag() const;

    void writeValue(double aValue);
    void writeValue(bool aValue);
    void writeValue(int aValue);

    bool readValue(double &rValue);
    bool readValue(bool &rValue);
    bool readValue(int &rValue);
signals:
    void valueChanged(double);
    void valueChanged(bool);
    void valueChanged(int);

private slots:
    void onTagValueChanged(Tag* aTag);
    void onTagCreated();

private:
    Tag *mTag;
    QString mTagName;

    QString mSubSystem;
    QString mName;
    Type mType;
};

#endif // TAGSOCKET_H
