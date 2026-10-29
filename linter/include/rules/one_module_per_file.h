#pragma once

namespace SURELOG {
class FileContent;
class ErrorContainer;
class SymbolTable;
}  // namespace SURELOG

void CheckOneModulePerFile(const SURELOG::FileContent* fileContent,
                           SURELOG::ErrorContainer* errors,
                           SURELOG::SymbolTable* symbols);