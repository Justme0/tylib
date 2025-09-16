package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tylib",
    hdrs = glob(["tylib/**/*.h"]),
    srcs = glob(["tylib/time/timer.cc"]),
    copts = ["-Werror", "-Wall", "-Wextra"],
)

cc_test(
  name = "tylib_test",
  size = "small",
  srcs = glob([
    "tylib/**/*_test.cc",
    "tylib/**/*.h"
  ]),

  copts = ["-I tylib", "-Werror", "-Wall", "-Wextra"],

  defines = [
    "JSONCPP",
    "RAPIDJSON",
  ],

  deps = [
    "@rapidjson//:rapidjson",
    "@jsoncpp//:jsoncpp",
    "@googletest//:gtest",
    "@googletest//:gtest_main",
    "//:tylib",
  ],
)
