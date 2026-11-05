#include <Surelog/Library/Library.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "Surelog/API/Surelog.h"
#include "Surelog/CommandLine/CommandLineParser.h"
#include "Surelog/Design/Design.h"
#include "Surelog/Design/FileContent.h"
#include "Surelog/ErrorReporting/ErrorContainer.h"
#include "Surelog/Library/Library.h"
#include "Surelog/Library/LibrarySet.h"
#include "Surelog/SourceCompile/SymbolTable.h"
#include "main/lint_rules.h"
#include "utils/init.h"

namespace SL = SURELOG;

namespace fs = std::filesystem;

namespace {

auto GetDesignFromPath(const fs::path& path, SL::ErrorContainer* errors,
                       SL::SymbolTable* symbols) -> SL::Design* {
  const auto clp = std::make_unique<SURELOG::CommandLineParser>(errors, symbols,
                                                                false, false);
  InitCommandLineParser(clp.get());

  const std::string path_str = path.string();
  const size_t argc = 2;
  std::array<const char*, argc> argv = {"", path_str.c_str()};
  if (!clp->parseCommandLine(argc, argv.data())) {
    std::cerr << "Can't parse command line" << '\n';
  }

  try {
    const auto compiler = start_compiler(clp.get());
    const auto design = get_design(compiler);
    return design;
  } catch (const std::exception& e) {
    std::cerr << "Compiler error: " << e.what() << '\n';
    return nullptr;
  }
}

auto GetFileContentFromPath(const fs::path& path, SL::ErrorContainer* errors,
                            SL::SymbolTable* symbols) -> SL::FileContent* {
  const auto fileContentMap =
      GetDesignFromPath(path, errors, symbols)->getAllFileContents();
  return fileContentMap.at(0).second;
}
}  // namespace

namespace test {

void CheckWithNoErrorsExpected(
    const fs::path& tests_path,
    const std::function<void(const SL::FileContent*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func) {
  std::vector<fs::path> paths(fs::directory_iterator{tests_path},
                              fs::directory_iterator{});

  for (auto& file_path : paths) {
    std::cout << "TESTING FILE:" << file_path << '\n';
    auto symbols = std::make_unique<SURELOG::SymbolTable>();
    auto errors = std::make_unique<SURELOG::ErrorContainer>(symbols.get());

    const SL::FileContent* fC =
        GetFileContentFromPath(file_path, errors.get(), symbols.get());
    check_func(fC, errors.get(), symbols.get());
    errors->printMessages();
    ASSERT_EQ(errors.get()->getErrors().size(), 0);
  }
}

void CheckWithErrorsExpected(
    const fs::path& tests_path, verihogg_lint::LintIdEnum errorIdExpected,
    const std::unordered_set<SURELOG::ErrorDefinition::ErrorType>& ignoreList,
    const std::function<void(const SL::FileContent*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func) {
  std::vector<fs::path> paths(fs::directory_iterator{tests_path},
                              fs::directory_iterator{});

  for (auto& file_path : paths) {
    std::cout << "TESTING FILE:" << file_path << "\n";
    auto symbols = std::make_unique<SURELOG::SymbolTable>();
    auto errors = std::make_unique<SURELOG::ErrorContainer>(symbols.get());

    const SL::FileContent* fC =
        GetFileContentFromPath(file_path, errors.get(), symbols.get());
    check_func(fC, errors.get(), symbols.get());
    errors->printMessages();

    auto errorVector = errors.get()->getErrors();
    ASSERT_NE(errorVector.size(), 0);
    bool hasError = false;
    for (auto& error : errorVector) {
      SURELOG::ErrorDefinition::ErrorType type = error.getType();
      if (ignoreList.count(type) > 0) {
        continue;
      }
      hasError = true;
      ASSERT_EQ(type, errorIdExpected);
    }
    ASSERT_EQ(hasError, true);
  }
}
}  // namespace test

namespace global {
void CheckWithNoErrorsExpected(
    const fs::path& tests_path,
    const std::function<void(SL::Design*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func) {
  std::vector<fs::path> paths(fs::directory_iterator{tests_path},
                              fs::directory_iterator{});

  for (auto& file_path : paths) {
    std::cout << "TESTING FILE:" << file_path << '\n';
    auto symbols = std::make_unique<SURELOG::SymbolTable>();
    auto errors = std::make_unique<SURELOG::ErrorContainer>(symbols.get());

    SL::Design* design =
        GetDesignFromPath(file_path, errors.get(), symbols.get());
    check_func(design, errors.get(), symbols.get());
    errors->printMessages();
    ASSERT_EQ(errors.get()->getErrors().size(), 0);
  }
}

void CheckWithErrorsExpected(
    const fs::path& tests_path, verihogg_lint::LintIdEnum errorIdExpected,
    const std::unordered_set<SURELOG::ErrorDefinition::ErrorType>& ignoreList,
    const std::function<void(SL::Design*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func) {
  std::vector<fs::path> paths(fs::directory_iterator{tests_path},
                              fs::directory_iterator{});

  for (auto& file_path : paths) {
    std::cout << "TESTING FILE:" << file_path << "\n";
    auto symbols = std::make_unique<SURELOG::SymbolTable>();
    auto errors = std::make_unique<SURELOG::ErrorContainer>(symbols.get());

    SL::Design* design =
        GetDesignFromPath(file_path, errors.get(), symbols.get());
    const auto libs = design->getLibrarySet()->getLibraries();
    std::cout << "libs size: " << libs.size() << "\n";

    check_func(design, errors.get(), symbols.get());
    errors->printMessages();

    auto errorVector = errors.get()->getErrors();
    ASSERT_NE(errorVector.size(), 0);
    bool hasError = false;
    for (auto& error : errorVector) {
      SURELOG::ErrorDefinition::ErrorType type = error.getType();
      if (ignoreList.count(type) > 0) {
        continue;
      }
      hasError = true;
      ASSERT_EQ(type, errorIdExpected);
    }
    ASSERT_EQ(hasError, true);
  }
}

void CheckSingleFileWithErrorsExpected(
    const fs::path& file_path, verihogg_lint::LintIdEnum errorIdExpected,
    const std::unordered_set<SURELOG::ErrorDefinition::ErrorType>& ignoreList,
    const std::function<void(SL::Design*, SL::ErrorContainer*,
                             SL::SymbolTable*)>& check_func) {
  std::cout << "TESTING FILE:" << file_path << "\n";
  auto symbols = std::make_unique<SURELOG::SymbolTable>();
  auto errors = std::make_unique<SURELOG::ErrorContainer>(symbols.get());

  SL::Design* design =
      GetDesignFromPath(file_path, errors.get(), symbols.get());
  check_func(design, errors.get(), symbols.get());
  errors->printMessages();

  auto errorVector = errors.get()->getErrors();
  ASSERT_NE(errorVector.size(), 0);
  bool hasError = false;
  for (auto& error : errorVector) {
    SURELOG::ErrorDefinition::ErrorType type = error.getType();
    if (ignoreList.count(type) > 0) {
      continue;
    }
    hasError = true;
    ASSERT_EQ(type, errorIdExpected);
  }
  ASSERT_EQ(hasError, true);
}
}  // namespace global
