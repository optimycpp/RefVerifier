//=--RefDiagConsumer.h--------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The main interface for invoking RefVerifier tool.
// This provides various methods that can be used to access different
// aspects of the RefVerifier tool.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/VerifyDiagnosticConsumer.h"
#include "clang/RefVerifier/ProjectInfo.h"

using namespace clang;
#ifndef LLVM_CLANG_REFVERIFIER_REFDIAGCONSUMER_H
#define LLVM_CLANG_REFVERIFIER_REFDIAGCONSUMER_H

class RefDiagConsumer: public DiagnosticConsumer {
public:
  RefDiagConsumer(ProjectInfo &P) : PI(P) { }
  virtual ~RefDiagConsumer() { }
  void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                        const Diagnostic &Info) override;
private:
  ProjectInfo &PI;

};

#endif //LLVM_CLANG_REFVERIFIER_REFDIAGCONSUMER_H
