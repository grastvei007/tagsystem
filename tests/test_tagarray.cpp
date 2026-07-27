#include <gtest/gtest.h>

#include <initializer_list>

#include <QString>
#include <QList>

#include <tag.h>
#include <taglist.h>


struct TestTagParam
{
	TagType type;
	QString subsystem;
	QString name;
	QVariant value;
	QString description;
	bool isArray;
};


class TestTagParamArray : public ::testing::TestWithParam<TestTagParam>
{
  public:
	struct PrintToStringParamName
	{
		template <class ParamType>
		std::string operator()( const testing::TestParamInfo<ParamType>& info ) const
		{
			auto tagParam = static_cast<TestTagParam>(info.param);
			return Tag::toString(tagParam.type).toStdString();
		}
	};

  protected:
	 void SetUp() override {

	}

	void TearDown() override {
		TagList::sGetInstance().clear();
	}

};

INSTANTIATE_TEST_SUITE_P(TagTypeParameters, TestTagParamArray,
						 ::testing::Values(  TestTagParam{.type = TagType::eBool,
														.subsystem = "subsystem",
														.name = "name",
														.value = QVariantList{true, true, false, true, false},
														.description = "desc",
														.isArray = true},
										   TestTagParam{.type = TagType::eDouble,
														.subsystem = "subsystem",
														.name = "name",
														.value = QVariantList{10.0, 11.0, 12.0, 13.0, 14.0},
														.description = "desc",
														.isArray = true},
										   TestTagParam{.type = TagType::eInt,
														.subsystem = "subsystem",
														.name = "name",
														.value = QVariantList{11, 12, 13, 14, 15},
														.description = "desc",
														.isArray = true},
										   TestTagParam{.type = TagType::eString,
														.subsystem = "subsystem",
														.name = "name",
														.value = QVariantList{"test", "test1", "test2", "test3", "test4"},
														.description = "desc",
														.isArray = true},
										   TestTagParam{.type = TagType::eTime,
														.subsystem = "subsystem",
														.name = "name",
														.value = QVariantList{QDateTime::currentDateTime(), QDateTime::currentDateTime().addDays(1), QDateTime::currentDateTime().addDays(2), QDateTime::currentDateTime().addDays(3), QDateTime::currentDateTime().addDays(4) },
														.description = "desc",
														.isArray = true}),
						 TestTagParamArray::PrintToStringParamName());


TEST_P(TestTagParamArray, createArrayTags_verifySize) {
	TestTagParam param = GetParam();
	auto *tag =TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value, param.description, param.isArray);

	EXPECT_EQ(tag->size(), param.value.toList().size());
}
