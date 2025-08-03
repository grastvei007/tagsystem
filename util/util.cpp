#include "util.h"

namespace util {

bool isTagCompatibleWithTagSocket(Tag::Type tagType, TagSocket::Type tagSocketType)
{
    if(tagType == Tag::Type::eBool && tagSocketType == TagSocket::Type::eBool)
        return true;

    if(tagType == Tag::Type::eDouble && tagSocketType == TagSocket::Type::eDouble)
        return true;

    if(tagType == Tag::Type::eInt && tagSocketType == TagSocket::Type::eInt)
        return true;

    if(tagType == Tag::Type::eString && tagSocketType == TagSocket::Type::eString)
        return true;

    if(tagType == Tag::Type::eTime && tagSocketType == TagSocket::Type::eTime)
        return true;

    return false;
}

} // end namespace
