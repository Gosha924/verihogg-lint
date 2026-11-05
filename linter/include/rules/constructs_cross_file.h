#pragma once

namespace SURELOG {
class Design;
class ErrorContainer;
class SymbolTable;
}  // namespace SURELOG

void CheckConstructsCrossFile(SURELOG::Design* design,
                              SURELOG::ErrorContainer* errors,
                              SURELOG::SymbolTable* symbols);