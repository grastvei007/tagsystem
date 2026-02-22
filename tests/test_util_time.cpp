#include <gtest/gtest.h>

#include <util/time.h>

#include <QTime>

class TestUtilTime : public ::testing::Test
{};

TEST_F(TestUtilTime, verify_timeSlot_onlyCaptureOnce)
{
	const int timeSlotMS = 1000;
	auto timeToVerify = QTime(1, 0).msecsSinceStartOfDay();

	EXPECT_TRUE(util::time::isInTimeSlot(1, 0, timeSlotMS, timeToVerify));

	EXPECT_FALSE(util::time::isInTimeSlot(1, 0, timeSlotMS, timeToVerify + timeSlotMS));
}
