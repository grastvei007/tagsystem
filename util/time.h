#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <QTime>

namespace util::time {

bool isInTimeSlot(int hour,
				  int min,
				  int timeSlotMs,
				  int timeToCheckMs = QTime::currentTime().msecsSinceStartOfDay());

} // namespace util::time

#endif // TIME_H
