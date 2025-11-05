#ifndef UTIL_H
#define UTIL_H

#include "tag.h"
#include "tagsocket.h"

namespace util
{
bool isTagCompatibleWithTagSocket(TagType tagType, TagSocket::Type tagSocketType);

} // end namespace

#endif // TAG_H
