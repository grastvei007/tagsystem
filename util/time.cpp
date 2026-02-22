#include "time.h"

#include <QTime>

namespace util::time {

bool isInTimeSlot(int hour, int min, int timeSlotMs, int timeToCheckMs)
{
	auto startOfTimeSlot = QTime(hour, min).msecsSinceStartOfDay();

	return (timeToCheckMs >= startOfTimeSlot && timeToCheckMs < (startOfTimeSlot + timeSlotMs));
}

} // namespace util::time
