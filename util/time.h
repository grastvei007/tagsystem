#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <QTime>
#include "date.h"

namespace util::time {

bool isInTimeSlot(int hour,
				  int min,
				  int timeSlotMs,
				  int timeToCheckMs = QTime::currentTime().msecsSinceStartOfDay());



bool hasDayChanged(util::date::DayOfWeek day);
} // namespace util::time

#endif // TIME_H
