#include <list>

#include "anytostring.h"
#include "gtest/gtest.h"

#include "example.pb.h"
#include "example_jce.h"

using namespace anytostring;

class Dress {
 private:
  uint64_t uin_ = 0;
  std::string id_ = "defaultId";

 public:
  std::string ToString() const {
    std::ostringstream oss;
    oss << "{uin=" << uin_ << ", id=" << id_ << "}";
    return oss.str();
  }
};

TEST(AnyToString, UserDefinedType) {
  std::vector<Dress> obj(2);
  EXPECT_EQ(AnyToString(obj), "[{uin=0, id=defaultId}, {uin=0, id=defaultId}]");
}

TEST(AnyToString, LiteralType) { EXPECT_EQ(AnyToString(123), "123"); }

TEST(AnyToString, Pair) {
  auto obj = std::make_pair(3, "hello");
  EXPECT_EQ(AnyToString(obj), "{3, hello}");
}

TEST(AnyToString, BuiltInVarType) {
  std::map<int, std::list<std::string>> obj = {{1, {"A", "BC"}}};
  EXPECT_EQ(AnyToString(obj), "[{1, [A, BC]}]");
}

TEST(AnyToString, JCE) {
  JceModule::User obj;
  obj.uid = "Mike";
  obj.age = 10;
  EXPECT_EQ(AnyToString(obj), "uid: Mike\nage: 10\n");
}

#if GOOGLE_PROTOBUF_VERSION >= 2000000
TEST(AnyToString, Protobuf) {
  apollo::STExample obj;
  obj.add_list(123);
  obj.add_list(456);
  EXPECT_EQ(AnyToString(obj), "list: 123\nlist: 456\n");
}
#endif

#if GOOGLE_PROTOBUF_VERSION >= 3000000
TEST(AnyToString, UserDefinedContainer) {
  apollo::STExampleMap pbMap;
  (*pbMap.mutable_name())[321] = "Ted";

  EXPECT_EQ(AnyToString(pbMap.name()), "[{321, Ted}]");
}
#endif

TEST(AnyToString, JsonCPP) {
  Json::Value obj;
  obj["data"]["value"] = 20;
  EXPECT_EQ(AnyToString(obj), "{\"data\":{\"value\":20}}\n");
}

TEST(AnyToString, RapidJSON) {
  rapidjson::Document obj;
  obj.SetObject();
  rapidjson::Document::AllocatorType& allctr = obj.GetAllocator();
  rapidjson::Value jData(rapidjson::kObjectType);
  jData.AddMember("value", 28, allctr);
  obj.AddMember("data", jData, allctr);
  EXPECT_EQ(AnyToString(obj), "{\"data\":{\"value\":28}}");
}

TEST(AnyToString, RawArray) {
  std::map<int32_t, std::array<int, 2>> array[2] = {{{1, {0, 4}}},
                                                    {{2, {9, 5}}}};
  EXPECT_EQ(AnyToString(array), "[[{1, [0, 4]}], [{2, [9, 5]}]]");
}

GTEST_API_ int main(int argc, char** argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
