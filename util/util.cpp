#include "util.h"

namespace util {

bool isTagCompatibleWithTagSocket(TagType tagType, TagSocket::Type tagSocketType)
{
    if(tagType == TagType::eBool && tagSocketType == TagSocket::Type::eBool)
        return true;

    if(tagType == TagType::eDouble && tagSocketType == TagSocket::Type::eDouble)
        return true;

    if(tagType == TagType::eInt && tagSocketType == TagSocket::Type::eInt)
        return true;

    if(tagType == TagType::eString && tagSocketType == TagSocket::Type::eString)
        return true;

    if(tagType == TagType::eTime && tagSocketType == TagSocket::Type::eTime)
        return true;

    return false;
}

} // end namespace
