#include <gtest/gtest.h>

#include <QDebug>
#include <QObject>

#include <tag2.h>
#include <private/tagtemplate.h>

#include <vector>

using namespace tagsystem;

class TestTag2 : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {
  //      TagList::sGetInstance().clear();
    }
};

TEST_F(TestTag2, create_tags)
{

    Tag2<double> tag(QString("d"), QString("f"));
    tag.setValue(10.2);

    EXPECT_EQ(tag.value(), 10.2);

    Tag<TagType::eDouble> tag2(QString("ds"), QString("sd"));
    tag2.setValue(3.3);
    EXPECT_EQ(tag2.value(), 3.3);

    EXPECT_EQ(tag2.type(), TagType::eDouble);
}

TEST_F(TestTag2, tags_in_list)
{
    /*  QObject::connect(tag, &QTag2Base::valueChanged, [tag](QTag2Base* a){
        qDebug() << "Tag value changed on tag: " << tag->value();
    });*/

    Tag<TagType::eDouble> t = Tag<TagType::eDouble>(QString("ds"), QString("sd"));
    Tag<TagType::eInt> t2 = Tag<TagType::eInt>(QString("test"), QString("name"));
    t.setValue(4.4);

    std::vector<TagVariant> tags;
    TagVariant vdv = t;

    auto tt = makeTag<TagType::eDouble>("ddf", "df");
    TagVariant v(tt);
}
