#include <gtest/gtest.h>

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

#include <map>

#include <tag.h>
#include <taglist.h>

class TestTag : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {
        TagList::sGetInstance().clear();
    }

    Tag* createIntTagWithValue(int value)
    {
        return TagList::sGetInstance().createTag(subsystem, name, type, value, desciption);
    }

    void setEnumNamesToTag(Tag *tag)
    {
        Tag::EnumMap map;
        map.emplace(1, "one");
        map.emplace(2, "two");
        map.emplace(3, "three");
        tag->setEnumValues(map);
    }

    void verifyEnumValuesArray(const QJsonArray &array)
    {
        EXPECT_EQ(array.size(), 3);

        for(auto &ref : array)
        {
            const QJsonObject& enumValue = ref.toObject();
            int key = enumValue.value("key").toInt();
            if(key == 1)
                EXPECT_EQ(enumValue.value("value").toString(), "one");
            else if(key == 2)
                EXPECT_EQ(enumValue.value("value").toString(), "two");
            else if(key == 3)
                EXPECT_EQ(enumValue.value("value").toString(), "three");
            else
                EXPECT_TRUE(false);
        }
    }

    QString subsystem{"testsubsystem"};
    QString name{"testname"};
    TagType type = TagType::eInt;
    QString desciption{"description"};
};


TEST_F(TestTag, enumNames_toJson_verifyEnumValues)
{
    auto *tag = createIntTagWithValue(10);
    setEnumNamesToTag(tag);

    QJsonObject json = tag->toJson();
    EXPECT_TRUE(json.contains("enumvalues"));

    const QJsonArray enumValues = json.value("enumvalues").toArray();

    verifyEnumValuesArray(enumValues);
}

TEST_F(TestTag, constructTagWithEnumValuesFromJson_verifyValuesIsCorrect)
{
    auto *tag = createIntTagWithValue(10);
    setEnumNamesToTag(tag);

    QJsonObject json = tag->toJson();
    QString newName("something_new");
    // replace tag name to force a new tag creation
    json.insert("name", newName);

    TagList::sGetInstance().UpdateOrCreateTag(json);

    auto *newTag = TagList::sGetInstance().findByTagName(subsystem, newName);
    EXPECT_TRUE(newTag);

    QJsonObject newTagJson = newTag->toJson();
    EXPECT_TRUE(newTagJson.contains("enumvalues"));

    const QJsonArray enumValues = newTagJson.value("enumvalues").toArray();

    verifyEnumValuesArray(enumValues);
}

TEST_F(TestTag, enumNamesForIntTag)
{
    auto *tag = createIntTagWithValue(1);
    setEnumNamesToTag(tag);

    EXPECT_TRUE(tag->enumValue(0).isEmpty());
    EXPECT_EQ(tag->enumValue(1), "one");
    EXPECT_EQ(tag->enumValue(2), "two");
    EXPECT_EQ(tag->enumValue(3), "three");
    EXPECT_TRUE(tag->enumValue(4).isEmpty());
}


TEST_F(TestTag, createTag)
{

    auto *tag = createIntTagWithValue(10);

    EXPECT_EQ(subsystem, tag->getSubsystem());
    EXPECT_EQ(name, tag->getName());
    EXPECT_EQ(10, tag->getIntValue());
    EXPECT_EQ(desciption, tag->getDescription());

    EXPECT_EQ(tag->getDoubleValue(), 0.0);
    EXPECT_EQ(tag->getBoolValue(), false);
    EXPECT_TRUE(tag->getStringValue().isEmpty());

    // new tag should be updated
    EXPECT_TRUE(tag->isUpdated());
}


struct TagParam
{
    TagType type;
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
        case TagType::eBool:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toBool(), param.description);
        case TagType::eDouble:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toDouble(), param.description);
        case TagType::eInt:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toInt(), param.description);
        case TagType::eString:
            return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toString(), param.description);
        case TagType::eTime:
			return TagList::sGetInstance().createTag(param.subsystem, param.name, param.type, param.value.toDateTime(), param.description);
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
        case TagType::eBool:
        {
            EXPECT_EQ(json.value("value").toBool(), tag->getBoolValue());
            break;
        }
        case TagType::eDouble:
        {
            EXPECT_EQ(json.value("value").toDouble(), tag->getDoubleValue());
            break;
        }
        case TagType::eInt:
        {
            EXPECT_EQ(json.value("value").toInt(), tag->getIntValue());
            break;
        }
        case TagType::eString:
        {
            EXPECT_EQ(json.value("value").toString(), tag->getStringValue());
            break;
        }
        case TagType::eTime:
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
    ::testing::Values(  TagParam{.type = TagType::eBool, .subsystem = "subsystem", .name = "name", .value = true, .description = "desc"},
                        TagParam{.type = TagType::eDouble, .subsystem = "subsystem", .name = "name", .value = 10.0, .description = "desc"},
                        TagParam{.type = TagType::eInt, .subsystem = "subsystem", .name = "name", .value = 11, .description = "desc"},
                        TagParam{.type = TagType::eString, .subsystem = "subsystem", .name = "name", .value = QString("test"), .description = "desc"},
                        TagParam{.type = TagType::eTime, .subsystem = "subsystem", .name = "name", .value = QDateTime::currentDateTime(), .description = "desc"}),
                         TestTagParameter::PrintToStringParamName());


TEST_P(TestTagParameter, TestParameter) {
    TagParam param = GetParam();

    auto *tag = createTag(param);
    EXPECT_TRUE(tag);

    QJsonObject obj = tag->toJson();
    validateJsonWithTag(obj, tag);
}

TEST_P(TestTagParameter, setValueToTag_expectValuChanged)
{
    TagParam param = GetParam();
    auto *tag = createTag(param);
    tag->resetUpdateFlag();

    switch (tag->getType())
    {
    case TagType::eBool:
    {
        bool currentValue = tag->getBoolValue();
        tag->setValue(!currentValue);
        EXPECT_EQ(!currentValue, tag->getBoolValue());

        EXPECT_EQ(tag->getIntValue(), 0);
        EXPECT_EQ(tag->getDoubleValue(), 0.0);
        EXPECT_EQ(tag->getTimeValue(), QDateTime::fromMSecsSinceEpoch(0));
        EXPECT_TRUE(tag->getStringValue().isEmpty());
        break;
    }
    case TagType::eDouble:
    {
        double value = tag->getDoubleValue() + 10.0;
        tag->setValue(value);
        EXPECT_EQ(tag->getDoubleValue(), value);

        EXPECT_EQ(tag->getIntValue(), 0);
        EXPECT_FALSE(tag->getBoolValue());
        EXPECT_EQ(tag->getTimeValue(), QDateTime::fromMSecsSinceEpoch(0));
        EXPECT_TRUE(tag->getStringValue().isEmpty());
        break;
    }
    case TagType::eInt:
    {
        int value = tag->getIntValue() + 10;
        tag->setValue(value);
        EXPECT_EQ(tag->getIntValue(), value);

        EXPECT_EQ(tag->getDoubleValue(), 0.0);
        EXPECT_FALSE(tag->getBoolValue());
        EXPECT_EQ(tag->getTimeValue(), QDateTime::fromMSecsSinceEpoch(0));
        EXPECT_TRUE(tag->getStringValue().isEmpty());
        break;
    }
    case TagType::eString:
    {
        QString value("super");
        tag->setValue(value);
        EXPECT_EQ(tag->getStringValue(), value);

        EXPECT_EQ(tag->getDoubleValue(), 0.0);
        EXPECT_EQ(tag->getIntValue(), 0);
        EXPECT_FALSE(tag->getBoolValue());
        EXPECT_EQ(tag->getTimeValue(), QDateTime::fromMSecsSinceEpoch(0));
        break;
    }
    case TagType::eTime:
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        tag->setValue(now);
        EXPECT_EQ(tag->getTimeValue().toMSecsSinceEpoch(), now);

        EXPECT_EQ(tag->getDoubleValue(), 0.0);
        EXPECT_EQ(tag->getIntValue(), 0);
        EXPECT_FALSE(tag->getBoolValue());
        EXPECT_TRUE(tag->getStringValue().isEmpty());
        break;
    }
    defalut:
        EXPECT_TRUE(false) << "Invalid tag type";
        break;
    }

    EXPECT_TRUE(tag->isUpdated());
}
