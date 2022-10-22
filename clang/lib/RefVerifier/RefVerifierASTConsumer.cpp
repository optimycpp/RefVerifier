//=--DetectERRASTConsumer.cpp-------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implementation of visitor methods for the DetectERRASTConsumer class. These
// visitors run various techniques for each function.
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/RefVerifierASTConsumer.h"
#include "clang/RefVerifier/FuncInfoVisitor.h"
#include "clang/RefVerifier/Utils.h"
#include "clang/Analysis/CFG.h"

using namespace llvm;
using namespace clang;

void RefVerifierASTConsumer::HandleTranslationUnit(ASTContext &C) {
  TranslationUnitDecl *TUD = C.getTranslationUnitDecl();

  // Iterate through all function declarations.
  for (const auto &D : TUD->decls()) {
    if (const FunctionDecl *FD = dyn_cast_or_null<FunctionDecl>(D)) {
      if (A == FUNC_INFO) {
        // Call function info visitor.
        FuncInfoVisitor RNV(&C, Info);
        if (Opts.Verbose) {
          llvm::outs() << "[+] Running FuncInfo Visitor handler.\n";
        }
        RNV.TraverseDecl(const_cast<FunctionDecl *>(FD));
      }
    }
  }
  return;
}