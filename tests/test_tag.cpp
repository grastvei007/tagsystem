#include <gtest/gtest.h>

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QJsonObject>

#include <tag.h>
#include <taglist.h>

class TestTag : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {
        TagList::sGetInstance().clear();
    }
};

TEST_F(TestTag, createTag)
{
    QString subsystem("testsubsystem");
    QString name("testname");
    Tag::Type type = Tag::eInt;
    int value = 10;
    QString desciption("description");
    auto *tag = TagList::sGetInstance().createTag(subsystem, name, type, value, desciption);

    EXPECT_EQ(subsystem, tag->getSubsystem());
    EXPECT_EQ(name, tag->getName());
    EXPECT_EQ(value, tag->getIntValue());
    EXPECT_EQ(desciption, tag->getDescription());

    EXPECT_EQ(tag->getDoubleValue(), 0.0);
    EXPECT_EQ(tag->getBoolValue(), false);
    EXPECT_TRUE(tag->getStringValue().isEmpty());

    // new tag should be updated
    EXPECT_TRUE(tag->isUpdated());
}


struct TagParam
{
    Tag::Type type;
    QString subsystem;
    QString name;
    QVariant value;
    QString description;
};


class TestTagParameter : public ::testing::TestWithParam<TagParam>
{
public:
    struct PrintToStringParamName
    {
        template <class ParamType>
        std::string operator()( const testing::TestParamInfo<ParamType>& info ) const
        {
            auto tagParam = static_cast<TagParam>(info.param);
            return Tag::toString(tagParam.type).toStdString();
        }
    };
protected:
    void SetUp() override {

    }

    void TearDown() override {
        TagList::sGetInstance().clear();
    }

    Tag * createTag(TagParam &param)
    {
        switch(param.type)
        {
        case Tag::eBool:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toBool(), param.description);
        case Tag::eDouble:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toDouble(), param.description);
        case Tag::eInt:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toInt(), param.description);
        case Tag::eString:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toString(), param.description);
        case Tag::eTime:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toTime(), param.description);
        }

        return nullptr;
    }

    void validateJsonWithTag(const QJsonObject &json, Tag* tag)
    {
        EXPECT_EQ(json.value("subsystem").toString(), tag->getSubsystem());
        EXPECT_EQ(json.value("name").toString(), tag->getName());
        EXPECT_EQ(json.value("type").toString(), Tag::toString(tag->getType()));
        EXPECT_EQ(json.value("description").toString(), tag->getDescription());
        switch(tag->getType())
        {
        case Tag::eBool:
        {
            EXPECT_EQ(json.value("value").toBool(), tag->getBoolValue());
            break;
        }
        case Tag::eDouble:
        {
            EXPECT_EQ(json.value("value").toDouble(), tag->getDoubleValue());
            break;
        }
        case Tag::eInt:
        {
            EXPECT_EQ(json.value("value").toInt(), tag->getIntValue());
            break;
        }
        case Tag::eString:
        {
            EXPECT_EQ(json.value("value").toString(), tag->getStringValue());
            break;
        }
        case Tag::eTime:
        {
            EXPECT_EQ(json.value("value").toInteger(), tag->getTimeValue().toMSecsSinceEpoch());
            break;
        }
        defalut:
            break;
        }
    }
};


INSTANTIATE_TEST_SUITE_P(TagTypeParameters, TestTagParameter,
    ::testing::Values(  TagParam{.type = Tag::eBool, .subsystem = "subsystem", .name = "name", .value = true, .description = "desc"},
                        TagParam{.type = Tag::eDouble, .subsystem = "subsystem", .name = "name", .value = 10.0, .description = "desc"},
                        TagParam{.type = Tag::eInt, .subsystem = "subsystem", .name = "name", .value = 11, .description = "desc"},
                        TagParam{.type = Tag::eString, .subsystem = "subsystem", .name = "name", .value = QString("test"), .description = "desc"},
                        TagParam{.type = Tag::eTime, .subsystem = "subsystem", .name = "name", .value = QDateTime::currentDateTime(), .description = "desc"}),
                         TestTagParameter::PrintToStringParamName());


TEST_P(TestTagParameter, TestParameter) {
    TagParam param = GetParam();

    auto *tag = createTag(param);
    EXPECT_TRUE(tag);

    QJsonObject obj = tag->toJson();
    validateJsonWithTag(obj, tag);
}
