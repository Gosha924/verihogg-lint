#pragma once

namespace SURELOG {
class Design;
class ErrorContainer;
class SymbolTable;
}  // namespace SURELOG

void CheckNoDirectivesInElementFiles(SURELOG::Design* design,
                                     SURELOG::ErrorContainer* errors,
                                     SURELOG::SymbolTable* symbols);