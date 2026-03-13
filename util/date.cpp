#include "date.h"

#include <QDate>

namespace util::date {
DayOfWeek currentDay()
{
	auto currentDay = QDate::currentDate().dayOfWeek();
	return DayOfWeek(currentDay);
}

Month currentMonth()
{
	auto currentDate = QDate::currentDate();
	return Month(currentDate.month());
}

} // namespace util::date
