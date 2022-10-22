//=--FuncInfoVisitor.h--------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class represents visitors dealing with computing function info.
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/RefVerifierASTConsumer.h"
#include "clang/RefVerifier/Utils.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/Analyses/Dominators.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <algorithm>

using namespace llvm;
using namespace clang;

#ifndef LLVM_CLANG_FUNCINFOVISITOR_H
#define LLVM_CLANG_FUNCINFOVISITOR_H

class FuncInfoVisitor : public RecursiveASTVisitor<FuncInfoVisitor> {
public:
  explicit FuncInfoVisitor(ASTContext *Context,
                           ProjectInfo &I)
      : Context(Context), Info(I) {
  }

  bool VisitFunctionDecl(FunctionDecl *FD);

private:
  ASTContext *Context;
  ProjectInfo &Info;
};

#endif //LLVM_CLANG_FUNCINFOVISITOR_H
