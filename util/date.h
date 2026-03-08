#ifndef DATE_H
#define DATE_H

namespace util::date {

enum class DayOfWeek {
	eMonday = 1,
	eTuesday = 2,
	eWednesDay = 3,
	eThursday = 4,
	eFriday = 5,
	eSaturday = 6,
	eSunday = 7,
};

DayOfWeek currentDay();
} // namespace util::date

#endif // DATE_H
