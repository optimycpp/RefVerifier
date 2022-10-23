//
// Created by machiry on 10/16/21.
//

#include "clang/RefVerifier/Utils.h"
#include "clang/RefVerifier/PersistentSourceLoc.h"
#include "clang/AST/Expr.h"

using namespace clang;

std::map<FuncDeclKey, FuncId> FuncIdMap::FuncDKeyToId;
// Initialize next function Id.
FuncId FuncIdMap::NextFuncId = 1;

FuncId FuncIdMap::getFuncID(const clang::FunctionDecl *FD, ASTContext *C) {
  std::string FN = FD->getNameAsString();
  if (const auto *CMD = dyn_cast<CXXMethodDecl>(FD)) {
    const auto *RDecl = CMD->getParent();
    FN = RDecl->getNameAsString() + "::" + CMD->getNameAsString();
  }
  bool IsSt = FD->isStatic();
  bool IsDecl = FD->isThisDeclarationADefinition();
  auto PSL = PersistentSourceLoc::mkPSL(FD, *C);

  FuncDeclKey FDKey (std::make_tuple(FN, PSL.getFileName(), IsSt, IsDecl,
                                    PSL.getLineNo()));

  if (FuncDKeyToId.find(FDKey) == FuncDKeyToId.end()) {
    FuncId PrevId;
    // Check if we found this function (declaration or definition before).
    if (FuncIdMap::getExistingFuncId(FN, IsSt, !IsDecl, PrevId)) {
      FuncDKeyToId[FDKey] = PrevId;
    } else {
      FuncDKeyToId[FDKey] = FuncIdMap::NextFuncId++;
    }
  }
  return FuncDKeyToId[FDKey];

}

bool FuncIdMap::getExistingFuncId(const std::string &FuncN,
                                  bool IsStatic, bool IsDecl,
                                  FuncId &CurrId) {
  for (const auto &MapElem: FuncIdMap::FuncDKeyToId) {
    const auto &FDeck = MapElem.first;
    if (std::get<0>(FDeck) == FuncN &&
        std::get<2>(FDeck) == IsStatic &&
        std::get<3>(FDeck) == IsDecl) {
      CurrId = MapElem.second;
      return true;
    }
  }
  return false;
}


FuncId getFuncID(const clang::FunctionDecl *FD, ASTContext *C) {
  return FuncIdMap::getFuncID(FD, C);
}

bool isInterestingType(const clang::Type *P) {
  return (P != nullptr &&
          P->isRecordType() &&
          !P->isReferenceType());
}

const Expr *removeAuxillaryCasts(const Expr *E) {
  bool NeedStrip = true;
  while (NeedStrip) {
    NeedStrip = false;
    E = E->IgnoreParenImpCasts();
    if (const CStyleCastExpr *C = dyn_cast<CStyleCastExpr>(E)) {
      E = C->getSubExpr();
      NeedStrip = true;
    }
  }
  return E;
}

bool isNULLExpr(const clang::Expr *E, ASTContext &C) {
  QualType Typ = E->getType();
  E = removeAuxillaryCasts(E);
  return Typ->isPointerType() && E->isIntegerConstantExpr(C) &&
         E->isNullPointerConstant(C, Expr::NPC_ValueDependentIsNotNull);
}

bool isNegativeNumber(const clang::Expr *E, ASTContext &C) {
  E = removeAuxillaryCasts(E);
  if (E->isIntegerConstantExpr(C)) {
    auto NewAPI = E->getIntegerConstantExpr(C);
    if (NewAPI.hasValue()) {
      return (NewAPI->getSExtValue() < 0);
    }
  }
  return false;
}