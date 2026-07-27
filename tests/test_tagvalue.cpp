#include <gtest/gtest.h>

#include <tagsystem/tagvalue.h>

class TestTagValue : public ::testing::Test
{
  protected:
	void SetUp() override {

	}

	void TearDown() override {

	}
};

TEST_F(TestTagValue, adfd)
{
	Value val;
	val.set<TagType::eInt>(10);

	EXPECT_EQ(val.is<TagType::eInt>(), true);
	EXPECT_EQ(val.get<TagType::eInt>(), 10);

}