#include "rules/no_directives_in_element_files.h"

#include <Surelog/Common/FileSystem.h>
#include <Surelog/Common/NodeId.h>
#include <Surelog/Common/PathId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string>
#include <string_view>

#include "main/lint_rules.h"
#include "utils/location_utils.h"

namespace SL = SURELOG;

namespace {

auto GetPath(SL::PathId fileId) -> std::string {
  if (!fileId) {
    return {};
  }
  return std::string(SL::FileSystem::getInstance()->toPath(fileId));
}

auto IsCheckedFile(SL::PathId fileId) -> bool {
  const std::string path = GetPath(fileId);
  return !path.empty() && std::string_view(path).ends_with(".svh");
}

auto CollectElementFileIds(SL::Design* design) -> SL::PathIdUnorderedSet {
  static const SL::VObjectTypeUnorderedSet kElementKinds{
      SL::VObjectType::paClass_declaration,
  };

  SL::PathIdUnorderedSet fileIds;
  for (const auto& [_, fileContent] : design->getAllFileContents()) {
    if (fileContent == nullptr) {
      continue;
    }

    for (const SL::NodeId node : fileContent->sl_collect_all(
             fileContent->getRootNode(), kElementKinds)) {
      const SL::PathId fileId = fileContent->getFileId(node);
      if (IsCheckedFile(fileId)) {
        fileIds.insert(fileId);
      }
    }
  }
  return fileIds;
}

}  // namespace

void CheckNoDirectivesInElementFiles(SL::Design* design,
                                     SL::ErrorContainer* errors,
                                     SL::SymbolTable* symbols) {
  if (design == nullptr) {
    return;
  }

  const SL::PathIdUnorderedSet elementFileIds = CollectElementFileIds(design);
  if (elementFileIds.empty()) {
    return;
  }

  for (auto& [_, ppContent] : design->getAllPPFileContents()) {
    if (ppContent == nullptr ||
        !elementFileIds.contains(ppContent->getFileId())) {
      continue;
    }

    const std::string path = GetPath(ppContent->getFileId());
    for (const SL::NodeId node :
         ppContent->sl_collect_all(ppContent->getRootNode(),
                                   SL::VObjectTypeUnorderedSet{
                                       SL::VObjectType::ppInclude_directive})) {
      ReportError(ppContent, node, path,
                  verihogg_lint::LINT_NO_DIRECTIVES_IN_ELEMENT_FILES, errors,
                  symbols);
    }
  }

  for (auto& [_, fileContent] : design->getAllFileContents()) {
    if (fileContent == nullptr) {
      continue;
    }

    for (SL::NodeId node : fileContent->sl_collect_all(
             fileContent->getRootNode(),
             SL::VObjectTypeUnorderedSet{
                 SL::VObjectType::paPackage_import_declaration})) {
      const SL::PathId fileId = fileContent->getFileId(node);
      if (!elementFileIds.contains(fileId)) {
        continue;
      }

      ReportError(fileContent, node, GetPath(fileId),
                  verihogg_lint::LINT_NO_DIRECTIVES_IN_ELEMENT_FILES, errors,
                  symbols);
    }
  }
}
