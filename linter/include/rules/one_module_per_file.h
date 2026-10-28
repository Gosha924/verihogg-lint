#pragma once

#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>

namespace SURELOG {
class FileContent;
}

void CheckOneModulePerFile(const SURELOG::FileContent* fileContent,
                           SURELOG::ErrorContainer* errors,
                           SURELOG::SymbolTable* symbols);