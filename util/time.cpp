#include "time.h"

#include <QTime>
#include <QDate>

namespace util::time {

bool isInTimeSlot(int hour, int min, int timeSlotMs, int timeToCheckMs)
{
	auto startOfTimeSlot = QTime(hour, min).msecsSinceStartOfDay();

	return (timeToCheckMs >= startOfTimeSlot && timeToCheckMs < (startOfTimeSlot + timeSlotMs));
}

bool hasDayChanged(date::DayOfWeek day)
{
	auto currentDate = QDate::currentDate();
	return currentDate.dayOfWeek() != static_cast<int>(day);
}

} // namespace util::time
