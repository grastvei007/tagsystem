#include <gtest/gtest.h>

#include <QString>

#include <tag.h>
#include <taglist.h>

TEST(TestTag, createTag)
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
