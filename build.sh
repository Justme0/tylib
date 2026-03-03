set -ex

find tylib | egrep ".+\.(cc|cpp|h)$" | xargs clang-format -i || true
# Bazel 9 removed native cc rules; autoload them for older BCR packages
bazel test --incompatible_autoload_externally=cc_library,cc_test,cc_binary --verbose_failures --sandbox_debug --subcommands --explain=bazel_build.log --verbose_explanations --cxxopt="-std=c++17" --test_output=all //:tylib_test
