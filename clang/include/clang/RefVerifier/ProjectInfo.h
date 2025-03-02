//=--ProjectInfo.h------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This class represents all the information about all the source files
// collected by the refverifier.
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/PersistentSourceLoc.h"
#include "clang/RefVerifier/Utils.h"

#ifndef LLVM_CLANG_REFVERIFIER_PROJECTINFO_H
#define LLVM_CLANG_REFVERIFIER_PROJECTINFO_H

// This stores global information about the project.
class ProjectInfo {
public:
  ProjectInfo() {

  }

  ~ProjectInfo() {
  }

  // For a given function id add the index of non-scalar argument.
  void addNSArg(const FuncId &FID, int NSArg, const ParmVarDecl *PVD);

  // Write the parsed function info to json.
  void dumpFuncIDInfoToJson(llvm::raw_ostream &O) const;

  bool parseFuncIDInfoJson(const std::string &JsonFP);

  void addErrorMessage(const std::string &FN, unsigned int LineNo,
                       const std::string &Msg);

  // Write the error info to json.
  void dumpErrorInfoToJson(llvm::raw_ostream &O) const;

private:
  std::map<FuncId, std::map<uint32_t, std::set<ParamRewriteKey>>> FuncIDNSArgs;
  std::map<std::string,
           std::set<std::pair<unsigned int, std::string>>> ErrMessages;
};

#endif //LLVM_CLANG_REFVERIFIER_PROJECTINFO_H
