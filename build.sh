set -ex

find tylib | egrep ".+\.(cc|cpp|h)$" | xargs clang-format -i || true
bazel test --verbose_failures --sandbox_debug --subcommands --explain=bazel_build.log --verbose_explanations --cxxopt="-std=c++17" --test_output=all //:tylib_test
