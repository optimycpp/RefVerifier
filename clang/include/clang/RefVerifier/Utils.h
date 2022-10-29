//=--Utils.h------------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Type declarations for map data structures and other general helper methods.
//===----------------------------------------------------------------------===//

#include "clang/AST/Decl.h"
#include "clang/AST/ASTContext.h"
#include "clang/RefVerifier/json.hpp"

#ifndef LLVM_CLANG_REFVERIFIER_UTILS_H
#define LLVM_CLANG_REFVERIFIER_UTILS_H

using namespace clang;
typedef unsigned long FuncId;

class FuncDeclKey {
public:
  std::string FuncName;
  std::string FileName;
  bool IsClassMethod;
  bool IsStatic;
  bool IsDefinition;
  bool IsVirtual;
  std::string ParentClass;
  std::string BaseClass;
  uint32_t StartLineNo;
  uint32_t EndLineNo;

  FuncDeclKey() { }

  FuncDeclKey (const FunctionDecl *FD, ASTContext *C);

  virtual ~FuncDeclKey() { }

  bool isSameFunctionDeclOrDefn(const FuncDeclKey &O) const;

  bool operator<(const FuncDeclKey &O) const {
    if (FuncName == O.FuncName)
      if (FileName == O.FileName)
        if (IsClassMethod == O.IsClassMethod)
          if (IsStatic == O.IsStatic)
            if (IsDefinition == O.IsDefinition)
              if (IsVirtual == O.IsVirtual)
                if (ParentClass == O.ParentClass)
                  if (BaseClass == O.BaseClass)
                    if (StartLineNo == O.StartLineNo)
                      if (EndLineNo == O.EndLineNo)
                        return false;
                      else
                        return EndLineNo < O.EndLineNo;
                    else
                      return StartLineNo < O.StartLineNo;
                  else
                    return BaseClass < O.BaseClass;
                else
                  return ParentClass < O.ParentClass;
              else
                return IsVirtual < O.IsVirtual;
            else
              return IsDefinition < O.IsDefinition;
          else
            return IsStatic < O.IsStatic;
        else
          return IsClassMethod < O.IsClassMethod;
      else
        return FileName < O.FileName;
    else
      return FuncName < O.FuncName;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(FuncDeclKey, FuncName, FileName,
                                 IsClassMethod, IsStatic, IsDefinition,
                                 IsVirtual, ParentClass, BaseClass,
                                 StartLineNo, EndLineNo)
};

class FuncIdMap {
public:
  static FuncId getFuncID(const clang::FunctionDecl *FD, ASTContext *C);

  static const std::map<FuncDeclKey, FuncId>& getFuncDKeyToId() {
    return FuncIdMap::FuncDKeyToId;
  }

  static void addFuncDecl(const FuncDeclKey &FD, const FuncId &ID) {
    FuncDKeyToId[FD] = ID;
  }

private:
  static FuncId NextFuncId;
  static bool getExistingFuncId(const FuncDeclKey &Curr, FuncId &Existing);
  static std::map<FuncDeclKey, FuncId> FuncDKeyToId;

};

// Get function id for the given function declaration.
FuncId getFuncID(const clang::FunctionDecl *FD, ASTContext *C);

// Is the expression a NULL pointer expression?
bool isNULLExpr(const clang::Expr *E, ASTContext &C);

// Is the expression a negative integer expression?
bool isNegativeNumber(const clang::Expr *E, ASTContext &C);

// Check if the type is a non-scalar and non-reference type.
bool isInterestingType(const clang::Type *P);


#endif //LLVM_CLANG_REFVERIFIER_UTILS_H
