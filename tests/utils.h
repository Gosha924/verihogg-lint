#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <functional>

#include "Surelog/CommandLine/CommandLineParser.h"
#include "Surelog/Design/Design.h"
#include "Surelog/Design/FileContent.h"
#include "Surelog/ErrorReporting/ErrorContainer.h"
#include "Surelog/SourceCompile/SymbolTable.h"
#include "main/lint_rules.h"

namespace SL = SURELOG;

namespace fs = std::filesystem;

namespace test {

void CheckWithErrorsExpected(
    const fs::path& tests_path, verihogg_lint::LintIdEnum errorIdExpected,
    const std::unordered_set<SURELOG::ErrorDefinition::ErrorType>& ignoreList,
    const std::function<void(const SL::FileContent*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func);

void CheckWithNoErrorsExpected(
    const fs::path& tests_path,
    const std::function<void(const SL::FileContent*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func);
}  // namespace test

namespace global {

void CheckWithErrorsExpected(
    const fs::path& tests_path, verihogg_lint::LintIdEnum errorIdExpected,
    const std::unordered_set<SURELOG::ErrorDefinition::ErrorType>& ignoreList,
    const std::function<void(SL::Design*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func);

void CheckWithNoErrorsExpected(
    const fs::path& tests_path,
    const std::function<void(SL::Design*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func);

void CheckSingleFileWithErrorsExpected(
    const fs::path& file_path, verihogg_lint::LintIdEnum errorIdExpected,
    const std::unordered_set<SURELOG::ErrorDefinition::ErrorType>& ignoreList,
    const std::function<void(SL::Design*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func);
}  // namespace global
