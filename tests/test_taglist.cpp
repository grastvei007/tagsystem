#include <gtest/gtest.h>

#include <taglist.h>


class TestTagList : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {
        TagList::sGetInstance().clear();
    }

    TagList& tagList() const
    {
        return TagList::sGetInstance();
    }

    int createTags()
    {
		TagList::sGetInstance().createTag("subsystem1", "name1", TagType::eInt, 10, "test1", false);
		TagList::sGetInstance().createTag("subsystem1", "name2", TagType::eInt, 20, "test2", false);
		TagList::sGetInstance().createTag("subsystem1", "name3", TagType::eInt, 30, "test3", false);
		TagList::sGetInstance().createTag("subsystem1", "name4", TagType::eInt, 40, "test4", false);
		TagList::sGetInstance().createTag("subsystem2", "name5", TagType::eInt, 50, "test5", false);
		TagList::sGetInstance().createTag("subsystem2", "name6", TagType::eInt, 60, "test6", false);
		TagList::sGetInstance().createTag("subsystem2", "name7", TagType::eInt, 70, "test7", false);
		TagList::sGetInstance().createTag("subsystem3", "name8", TagType::eInt, 80, "test8", false);
		TagList::sGetInstance().createTag("subsystem3", "name9", TagType::eInt, 90, "test9", false);
		TagList::sGetInstance().createTag("subsystem3", "name10", TagType::eInt, 100, "test10", false);

        return 10; // number of tags created
    }
};


TEST_F(TestTagList, createTags_verifyNumberOfTags)
{
    auto numberOfTags = createTags();
    EXPECT_EQ(numberOfTags, tagList().getNumberOfTags());
}

TEST_F(TestTagList, createTags_findTagByName_expectFound)
{
    QString subsystem("subsystem1");
    QString name("name3");
    createTags();
    auto *tag = tagList().findByTagName(subsystem, name);

    EXPECT_TRUE(tag);
    EXPECT_EQ(subsystem, tag->getSubsystem());
    EXPECT_EQ(name, tag->getName());
}

TEST_F(TestTagList, createTags_findTagsInSubsystem)
{
    createTags();

    auto subsystem1 = tagList().findTagsInSubsystem("subsystem1");
    auto subsystem2 = tagList().findTagsInSubsystem("subsystem2");
    auto subsystem3 = tagList().findTagsInSubsystem("subsystem3");
    auto nonExistingSubsystem = tagList().findTagsInSubsystem("none");

    EXPECT_EQ(subsystem1.size(), 4);
    EXPECT_EQ(subsystem2.size(), 3);
    EXPECT_EQ(subsystem3.size(), 3);
    EXPECT_TRUE(nonExistingSubsystem.empty());
}

TEST_F(TestTagList, createTags_createNewTagByJson)
{
    createTags();
    auto *tag = tagList().getTagByIndex(3);
    QJsonObject json = tag->toJson();
    json.insert("name", "test");
    tagList().UpdateOrCreateTag(json);

    EXPECT_EQ(tagList().getNumberOfTags(), 11);

    auto *newTag = tagList().findByTagName(tag->getSubsystem(), "test");
    EXPECT_TRUE(newTag);
}

TEST_F(TestTagList, createTag_updateValueOnTagByJson)
{
    createTags();
    auto *tag = tagList().getTagByIndex(3);
    QJsonObject json = tag->toJson();
    json.insert("value", 42);
    tagList().UpdateOrCreateTag(json);

    EXPECT_EQ(tag->getIntValue(), 42);
}
