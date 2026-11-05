#include "rules/constructs_cross_file.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <unordered_set>

#include "main/lint_rules.h"
#include "utils.h"

namespace fs = std::filesystem;

namespace {

auto BasePath() -> const fs::path {
  return fs::current_path() / ".." / ".." / "tests" / "ConstructChecks";
}

}  // namespace

TEST(ConstructsCrossFile, NoError) {
  const fs::path tests_path{BasePath() / "ConstructsCrossFile" / "NoError"};

  global::CheckWithNoErrorsExpected(tests_path, CheckConstructsCrossFile);
}

TEST(ConstructsCrossFile, RaiseError) {
  const fs::path main_path{BasePath() / "ConstructsCrossFile" / "RaiseError" /
                           "main.sv"};

  const std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{};

  global::CheckSingleFileWithErrorsExpected(
      main_path, verihogg_lint::LINT_CONSTRUCTS_CROSS_FILE, ignoreList,
      CheckConstructsCrossFile);
}