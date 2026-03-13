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

enum class Month {
	eJanuary = 1,
	eFebruary = 2,
	eMarch = 3,
	eApril = 4,
	eMay = 5,
	eJune = 6,
	eJuly = 7,
	eAugust = 8,
	eSeptember = 9,
	eOctober = 10,
	eNovember = 11,
	eDecember = 12
};

DayOfWeek currentDay();
Month currentMonth();

} // namespace util::date

#endif // DATE_H
