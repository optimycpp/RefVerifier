//=--FuncInfoVisitor.cpp------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This implementation of methods in FuncInfoVisitor.
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/FuncInfoVisitor.h"
#include "clang/RefVerifier/Utils.h"


bool FuncInfoVisitor::VisitFunctionDecl(FunctionDecl *FD) {
  auto FuncID = FuncIdMap::getFuncID(FD, this->Context);
  for (unsigned I = 0; I < FD->getNumParams(); I++) {
    auto *TypePtr = FD->getParamDecl(I)->getType().getTypePtrOrNull();
    // Check if the parameter type is interesting?
    if (isInterestingType(TypePtr)) {
      this->Info.addNSArg(FuncID, I);
    }
  }
  return true;
}
