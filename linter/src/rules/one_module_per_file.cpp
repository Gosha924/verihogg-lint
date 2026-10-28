#include "rules/one_module_per_file.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string>
#include <vector>

#include "main/lint_rules.h"
#include "utils/location_utils.h"

namespace SL = SURELOG;

namespace {

auto IsTopLevel(SL::NodeId nodeId, const SL::FileContent* fileContent,
                SL::NodeId rootNode) -> bool {
  SL::NodeId current = fileContent->Parent(nodeId);
  while (current != rootNode && current != SL::NodeId(0)) {
    const SL::VObjectType type = fileContent->Type(current);

    if (type == SL::VObjectType::paClass_declaration ||
        type == SL::VObjectType::paInterface_declaration ||
        type == SL::VObjectType::paPackage_declaration ||
        type == SL::VObjectType::paModule_declaration) {
      return false;
    }

    current = fileContent->Parent(current);
  }
  return true;
}

}  // namespace

void CheckOneModulePerFile(const SL::FileContent* fileContent,
                           SL::ErrorContainer* errors,
                           SL::SymbolTable* symbols) {
  if (fileContent == nullptr) {
    return;
  }

  const SL::NodeId rootNode = fileContent->getRootNode();

  const std::vector<SL::NodeId> classDecls = fileContent->sl_collect_all(
      rootNode, SL::VObjectType::paClass_declaration);
  const std::vector<SL::NodeId> interfaceDecls = fileContent->sl_collect_all(
      rootNode, SL::VObjectType::paInterface_declaration);
  const std::vector<SL::NodeId> packageDecls = fileContent->sl_collect_all(
      rootNode, SL::VObjectType::paPackage_declaration);
  const std::vector<SL::NodeId> moduleDecls = fileContent->sl_collect_all(
      rootNode, SL::VObjectType::paModule_declaration);

  int topLevelCount = 0;
  SL::NodeId firstTopLevelNode(0);
  for (const auto& classId : classDecls) {
    if (IsTopLevel(classId, fileContent, rootNode)) {
      topLevelCount++;
      if (firstTopLevelNode == SL::NodeId(0)) {
        firstTopLevelNode = classId;
      }
    }
  }

  for (const auto& interfaceId : interfaceDecls) {
    if (IsTopLevel(interfaceId, fileContent, rootNode)) {
      topLevelCount++;
      if (firstTopLevelNode == SL::NodeId(0)) {
        firstTopLevelNode = interfaceId;
      }
    }
  }

  for (const auto& packageId : packageDecls) {
    if (IsTopLevel(packageId, fileContent, rootNode)) {
      topLevelCount++;
      if (firstTopLevelNode == SL::NodeId(0)) {
        firstTopLevelNode = packageId;
      }
    }
  }

  for (const auto& moduleId : moduleDecls) {
    if (IsTopLevel(moduleId, fileContent, rootNode)) {
      topLevelCount++;
      if (firstTopLevelNode == SL::NodeId(0)) {
        firstTopLevelNode = moduleId;
      }
    }
  }

  if (topLevelCount > 1) {
    const std::string message =
        "file contains " + std::to_string(topLevelCount) +
        " top-level entities (class/interface/package/module), only one is allowed";
    ReportError(fileContent, firstTopLevelNode, message,
                verihogg_lint::LINT_ONE_MODULE_PER_FILE, errors, symbols);
  }
}