#include <gtest/gtest.h>

#include <QJsonObject>
#include <QJsonArray>

#include <tagsystem/clientinformation.h>


class TestClientnformation : public ::testing::Test
{
  protected:
	void SetUp() override {

	}

	void TearDown() override {

	}

	ClientInformation create(const QString &name){
		return ClientInformation(name);
	}
};

TEST_F(TestClientnformation, createDefaultClientInformation_verifyValues)
{
	auto ci = create("test");

	auto json = ci.getInfo().toLatin1();

	auto jsonObject = QJsonDocument::fromJson(json).object();

	EXPECT_TRUE(jsonObject.contains("name"));
	EXPECT_EQ(jsonObject.value("name").toString(), "test");
	EXPECT_TRUE(jsonObject.contains("ip"));
	EXPECT_EQ(jsonObject.size(), 2);


}

TEST_F(TestClientnformation, createClientIformationWithTagSet_verfyTagSet)
{
	auto ci = create("test");
	ci.setListenToTags({"test.tag1", "test.tag2"});

	auto json = ci.getInfo().toLatin1();

	auto jsonObject = QJsonDocument::fromJson(json).object();

	EXPECT_TRUE(jsonObject.contains("tag_set"));

	auto array = jsonObject.value("tag_set").toArray();

	EXPECT_EQ(array.size(), 2);

	EXPECT_EQ(array.at(0).toString(), "test.tag1");
	EXPECT_EQ(array.at(1).toString(), "test.tag2");
}
