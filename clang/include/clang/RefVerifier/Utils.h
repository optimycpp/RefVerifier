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

#ifndef LLVM_CLANG_REFVERIFIER_UTILS_H
#define LLVM_CLANG_REFVERIFIER_UTILS_H

using namespace clang;
typedef std::tuple<std::string, std::string, bool, bool, uint32_t> FuncDeclKey;
typedef unsigned long FuncId;

class FuncIdMap {
public:
  static FuncId getFuncID(const clang::FunctionDecl *FD, ASTContext *C);

  static const std::map<FuncDeclKey, FuncId>& getFuncDKeyToId() {
    return FuncIdMap::FuncDKeyToId;
  }

  static bool getFuncDeclKeyDecl(const FuncDeclKey &K) {
    return std::get<3>(K);
  }
  static const std::string &getFuncDeclKeyFuncName(const FuncDeclKey &K) {
    return std::get<0>(K);
  }
  static const std::string &getFuncDeclKeyFileName(const FuncDeclKey &K) {
    return std::get<1>(K);
  }

  static uint32_t getFuncDeclKeyLineNum(const FuncDeclKey &K) {
    return std::get<4>(K);
  }

private:
  static FuncId NextFuncId;
  static bool getExistingFuncId(const std::string &FuncN, bool IsStatic,
                                bool IsDecl, FuncId &CurrId);
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
