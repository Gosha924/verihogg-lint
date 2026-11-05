#include "rules/constructs_cross_file.h"

#include <Surelog/Common/FileSystem.h>
#include <Surelog/Common/NodeId.h>
#include <Surelog/Common/PathId.h>
#include <Surelog/Design/Design.h>
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

auto GetLeftLeaf(const SL::FileContent* fileContent, SL::NodeId node)
    -> SL::NodeId {
  SL::NodeId current = node;
  while (true) {
    const SL::NodeId child = fileContent->Child(current);
    if (child == SL::InvalidNodeId) {
      return current;
    }
    current = child;
  }
}

auto GetRightLeaf(const SL::FileContent* fileContent, SL::NodeId node)
    -> SL::NodeId {
  SL::NodeId current = node;
  while (true) {
    const SL::NodeId child = fileContent->Child(current);
    if (child == SL::InvalidNodeId) {
      return current;
    }

    current = child;
    for (SL::NodeId sibling = fileContent->Sibling(current);
         sibling != SL::InvalidNodeId;
         sibling = fileContent->Sibling(sibling)) {
      current = sibling;
    }
  }
}

auto GetPath(SL::PathId fileId) -> std::string {
  if (!fileId) {
    return {};
  }
  return std::string(SL::FileSystem::getInstance()->toPath(fileId));
}

}  // namespace

void CheckConstructsCrossFile(SL::Design* design, SL::ErrorContainer* errors,
                              SL::SymbolTable* symbols) {
  if (design == nullptr) {
    return;
  }

  static const SL::VObjectTypeUnorderedSet kConstructTypes{
      SL::VObjectType::paModule_declaration,
      SL::VObjectType::paPackage_declaration,
      SL::VObjectType::paProgram_declaration,
      SL::VObjectType::paInterface_declaration,
      SL::VObjectType::paClass_declaration,
      SL::VObjectType::paInterface_class_declaration,
      SL::VObjectType::paFunction_declaration,
      SL::VObjectType::paTask_declaration,
      SL::VObjectType::paClass_constructor_declaration,
  };

  for (const auto& entry : design->getAllFileContents()) {
    const SL::FileContent* fileContent = entry.second;
    if (fileContent == nullptr) {
      continue;
    }

    const std::vector<SL::NodeId> nodes = fileContent->sl_collect_all(
        fileContent->getRootNode(), kConstructTypes);

    for (const SL::NodeId node : nodes) {
      const SL::NodeId leftmost = GetLeftLeaf(fileContent, node);
      const SL::NodeId rightmost = GetRightLeaf(fileContent, node);

      if (leftmost == SL::InvalidNodeId || rightmost == SL::InvalidNodeId) {
        continue;
      }

      const SL::PathId nodeFile = fileContent->getFileId(node);
      const SL::PathId leftFile = fileContent->getFileId(leftmost);
      const SL::PathId rightFile = fileContent->getFileId(rightmost);

      if (!leftFile || !rightFile) {
        continue;
      }

      const bool crossesFileBoundary =
          leftFile != rightFile ||
          (nodeFile && (nodeFile != leftFile || nodeFile != rightFile));

      if (!crossesFileBoundary) {
        continue;
      }

      const SL::PathId startFile = nodeFile ? nodeFile : leftFile;
      ReportError(fileContent, node,
                  GetPath(startFile) + " -> " + GetPath(rightFile),
                  verihogg_lint::LINT_CONSTRUCTS_CROSS_FILE, errors, symbols);
    }
  }
}
