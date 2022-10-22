//=--DetectERRASTConsumer.h---------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/ProjectInfo.h"
#include "clang/RefVerifier/RefVerifier.h"
#include "clang/AST/ASTConsumer.h"

#ifndef LLVM_CLANG_REFVERIFIER_REFVERIFIERASTCONSUMER_H
#define LLVM_CLANG_REFVERIFIER_REFVERIFIERASTCONSUMER_H

using namespace clang;

enum ACTION_ENUM { FUNC_INFO, MODIFY_FUNC, VERIFY_FUNC};

// The main consumer that runs various techniques on each function.
class RefVerifierASTConsumer : public clang::ASTConsumer {
public:
  explicit RefVerifierASTConsumer(ProjectInfo &I, struct RefVerifierOptions &DOpts,
                                ASTContext *C, ACTION_ENUM ActType,
                                FuncId TFID = 0)
      : Info(I), Opts(DOpts), A(ActType), FID(TFID) { }

  void HandleTranslationUnit(ASTContext &) override;

private:
  ProjectInfo &Info;
  struct RefVerifierOptions &Opts;
  ACTION_ENUM A;
  FuncId FID;
};

#endif //LLVM_CLANG_REFVERIFIER_REFVERIFIERASTCONSUMER_H
