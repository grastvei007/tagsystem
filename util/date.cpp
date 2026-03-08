#include "date.h"

#include <QDate>

namespace util::date {
DayOfWeek currentDay()
{
	auto currentDay = QDate::currentDate().dayOfWeek();
	return DayOfWeek(currentDay);
}
} // namespace util::date
