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

ParamRewriteKey::ParamRewriteKey (const ParmVarDecl *PVD) {
  auto PSL = PersistentSourceLoc::mkTokenPSL(PVD, PVD->getASTContext());
  this->FileName = PSL.getFileName();
  this->TypeIndexLine = PSL.getLineNo();
  this->TypeIndexColumn = PSL.getColSNo();
  this->ParamIndexLine = PSL.getEndLineNo();
  this->ParamIndexColumn = PSL.getColENo();
}

FuncDeclKey::FuncDeclKey (const FunctionDecl *FD, ASTContext *C) {
  this->FuncName = FD->getNameAsString();
  if (const auto *CMD = dyn_cast<CXXMethodDecl>(FD)) {
    this->IsClassMethod = true;
    this->IsVirtual = CMD->isVirtual();
    auto *ParentDecl = CMD->getParent();
    this->ParentClass = ParentDecl->getNameAsString();
    this->BaseClass = "";
    if (ParentDecl->isClass()) {
      std::vector<const CXXRecordDecl*> TargetBases;
      TargetBases.clear();
      ParentDecl->forallBases([&TargetBases, CMD](const CXXRecordDecl *RD) {
        auto *BaseFn = CMD->getCorrespondingMethodDeclaredInClass(RD, true);
        if (BaseFn) {
          // we found it
          TargetBases.push_back(RD);
          // we can stop.
          return false;
        }
        return true;
      });
      if (!TargetBases.empty()) {
        this->BaseClass = (TargetBases.front())->getNameAsString();
      }
    }
  } else {
    this->IsClassMethod = false;
    this->ParentClass = "";
    this->BaseClass = "";
    this->IsVirtual = false;
  }
  auto PSL = PersistentSourceLoc::mkPSL(FD, *C);
  this->FileName = PSL.getFileName();
  this->StartLineNo = PSL.getLineNo();
  this->EndLineNo = PSL.getEndLineNo();
  this->IsStatic = FD->isStatic();
  this->IsDefinition = FD->isThisDeclarationADefinition();
}

bool FuncDeclKey::isSameFunctionDeclOrDefn(const FuncDeclKey &O) const {
  return this->FuncName == O.FuncName &&
         this->IsStatic == O.IsStatic &&
         this->IsVirtual == O.IsVirtual &&
         this->IsClassMethod == O.IsClassMethod &&
         this->ParentClass == O.ParentClass &&
         this->IsDefinition == !O.IsDefinition;
}

FuncId FuncIdMap::getFuncID(const clang::FunctionDecl *FD, ASTContext *C) {

  FuncDeclKey FDKey (FD, C);

  if (FuncDKeyToId.find(FDKey) == FuncDKeyToId.end()) {
    FuncId PrevId;
    // Check if we found this function (declaration or definition before).
    if (FuncIdMap::getExistingFuncId(FDKey, PrevId)) {
      FuncDKeyToId[FDKey] = PrevId;
    } else {
      FuncDKeyToId[FDKey] = FuncIdMap::NextFuncId++;
    }
  }
  return FuncDKeyToId[FDKey];

}

bool FuncIdMap::getExistingFuncId(const FuncDeclKey &Curr,
                                  FuncId &Existing) {
  for (const auto &MapElem: FuncIdMap::FuncDKeyToId) {
    if (MapElem.first.isSameFunctionDeclOrDefn(Curr)) {
      Existing = MapElem.second;
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