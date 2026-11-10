#include "rules/no_directives_in_element_files.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <unordered_set>

#include "main/lint_rules.h"
#include "utils.h"

namespace fs = std::filesystem;

namespace {

auto BasePath() -> const fs::path {
  return fs::current_path() / ".." / ".." / "tests" / "IncludeChecks";
}

}  // namespace

TEST(NoDirectivesInElementFiles, NoError) {
  const fs::path tests_path{BasePath() / "NoDirectivesInElementFiles" /
                            "NoError"};

  global::CheckWithNoErrorsExpected(tests_path,
                                    CheckNoDirectivesInElementFiles);
}

TEST(NoDirectivesInElementFiles, RaiseError) {
  const fs::path main_path{BasePath() / "NoDirectivesInElementFiles" /
                           "RaiseError" / "main.sv"};

  const std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{};

  global::CheckSingleFileWithErrorsExpected(
      main_path, verihogg_lint::LINT_NO_DIRECTIVES_IN_ELEMENT_FILES, ignoreList,
      CheckNoDirectivesInElementFiles);
}

TEST(NoDirectivesInElementFiles, ImportDirective) {
  const fs::path file_path{BasePath() / "NoDirectivesInElementFiles" /
                           "RaiseError" / "import.svh"};

  const std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{};

  global::CheckSingleFileWithErrorsExpected(
      file_path, verihogg_lint::LINT_NO_DIRECTIVES_IN_ELEMENT_FILES, ignoreList,
      CheckNoDirectivesInElementFiles);
}