#include "rules/one_module_per_file.h"

#include <gtest/gtest.h>

#include <filesystem>

#include "main/lint_rules.h"
#include "utils.h"

namespace fs = std::filesystem;

namespace {

auto BasePath() -> const fs::path {
  return fs::current_path() / ".." / ".." / "tests" / "FileChecks";
}
}  // namespace

TEST(OneModulePerFileTest, NoError) {
  const fs::path test_path{BasePath() / "OneModulePerFile" / "NoError"};

  test::CheckWithNoErrorsExpected(test_path, CheckOneModulePerFile);
}

TEST(OneModulePerFileTest, RaiseError) {
  const fs::path test_path{BasePath() / "OneModulePerFile" / "RaiseError"};

  const std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{};

  test::CheckWithErrorsExpected(test_path,
                                verihogg_lint::LINT_ONE_MODULE_PER_FILE,
                                ignoreList, CheckOneModulePerFile);
}