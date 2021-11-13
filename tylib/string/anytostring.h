#ifndef ANYTOSTRING_ANYTOSTRING_H_
#define ANYTOSTRING_ANYTOSTRING_H_

#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "jsoncpp/json.h"

#if GOOGLE_PROTOBUF_VERSION >= 2000000
#include "google/protobuf/message.h"
#endif

#if GOOGLE_PROTOBUF_VERSION >= 3000000
#include "google/protobuf/map.h"
#endif

#ifdef TENCENT_JCE
#include "jce/Jce.h"
#endif

namespace tylib {

inline const std::string& AnyToString(const std::string& s) { return s; }

inline std::string AnyToString(const Json::Value& jValue) {
  Json::FastWriter writer;
  return writer.write(jValue);
}

template <class J>
typename std::enable_if<std::is_base_of<::rapidjson::Value, J>::value,
                        std::string>::type
AnyToStringHelper(const J* jValue) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  jValue->Accept(writer);
  return buffer.GetString();
}

template <class T>
std::string AnyToString(const T& t);

template <class T1, class T2>
std::string AnyToString(const std::pair<T1, T2>& p) {
  std::stringstream ss;
  ss << "{" << AnyToString(p.first) << ", " << AnyToString(p.second) << "}";
  return ss.str();
}

#if GOOGLE_PROTOBUF_VERSION >= 3000000
// protobuf pair
template <class T1, class T2>
std::string AnyToString(const google::protobuf::MapPair<T1, T2>& p) {
  std::stringstream ss;
  ss << "{" << AnyToString(p.first) << ", " << AnyToString(p.second) << "}";
  return ss.str();
}
#endif

#if GOOGLE_PROTOBUF_VERSION >= 2000000
template <class Pb>
typename std::enable_if<std::is_base_of<::google::protobuf::Message, Pb>::value,
                        std::string>::type
AnyToStringHelper(const Pb* pb) {
  return pb->Utf8DebugString();
}
#endif

#ifdef TENCENT_JCE
template <class Jce>
typename std::enable_if<std::is_base_of<taf::JceStructBase, Jce>::value,
                        std::string>::type
AnyToStringHelper(const Jce* jce) {
  std::stringstream ss;
  jce->display(ss);
  return ss.str();
}
#endif

template <typename T>
class HasToString {
 private:
  typedef char YesType[1];
  typedef char NoType[2];

  template <typename C>
  static YesType& test(decltype(&C::ToString));
  template <typename C>
  static NoType& test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

template <typename T>
class HasBegin {
 private:
  typedef char YesType[1];
  typedef char NoType[2];

  template <typename C>
  struct ChT;

  template <typename C>
  static YesType& test(typename C::const_iterator*);
  template <typename C>
  static NoType& test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

template <typename T>
typename std::enable_if<HasToString<T>::value, std::string>::type
AnyToStringHelper(const T* t) {
  return t->ToString();
}

// print all element, maybe many :)
template <typename It>
std::string AnyToStringForRange(It begin, It end) {
  std::stringstream ss;
  ss << '[';
  for (It it = begin; it != end; ++it) {
    if (it != begin) {
      ss << ", ";
    }
    ss << AnyToString(*it);
  }
  ss << ']';

  return ss.str();
}

template <typename C>
typename std::enable_if<HasBegin<C>::value, std::string>::type
AnyToStringHelper(const C* c) {
  return AnyToStringForRange(c->begin(), c->end());
}

template <typename T, size_t N>
std::string AnyToString(T (&array)[N]) {
  return AnyToStringForRange(array, array + N);
}

template <typename T, typename... Arguments>
std::string AnyToStringHelper(const T* t, const Arguments&...) {
  std::stringstream ss;
  ss << *t;
  return ss.str();
}

template <class T>
std::string AnyToString(const T& t) {
  return AnyToStringHelper(&t);
}

}  // namespace tylib

#endif  // ANYTOSTRING_ANYTOSTRING_H_
