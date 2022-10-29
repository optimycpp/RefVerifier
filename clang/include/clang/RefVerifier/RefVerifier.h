//=--RefVerifier.h------------------------------------------------*- C++-*-===//
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

#ifndef LLVM_CLANG_REFVERIFIER_REFVERIFIER_H
#define LLVM_CLANG_REFVERIFIER_REFVERIFIER_H

#include "clang/RefVerifier/ProjectInfo.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include <mutex>

using namespace clang;

// Future, if we want to pass some options.
struct RefVerifierOptions {
  bool Verbose;
};

// The main interface exposed by the DetectERR to interact with the tool.
class RefVerifierInterface {
public:
  RefVerifierInterface(const struct RefVerifierOptions &DEopt,
                     const std::vector<std::string> &SourceFileList,
                     clang::tooling::CompilationDatabase *CompDB);

  bool computeAllFuncInfo();

  bool checkCompilerErrors();

  bool rewriteFuncParam(const std::string &FInfoJson,
                        uint32_t FuncId,
                        uint32_t ParamID);

  void dumpInfo(llvm::raw_ostream &O);

  void dumpCompileErrors(llvm::raw_ostream &O);

private:
  ProjectInfo PInfo;
  struct RefVerifierOptions RVeriOptions;
  tooling::CommandLineArguments SourceFiles;
  tooling::CompilationDatabase *CurrCompDB;
};

#endif // LLVM_CLANG_REFVERIFIER_REFVERIFIER_H
