//=--ProjectInfo.cpp----------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implementation of ProjectInfo methods.
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/ProjectInfo.h"

using namespace clang;

void ProjectInfo::addNSArg(const FuncId &FID, int NSArg) {
  this->FuncIDNSArgs[FID].insert(NSArg);
}
void ProjectInfo::dumpFuncIDInfoToJson(llvm::raw_ostream &O) const {
  std::map<FuncId, FuncDeclKey> DeclFuncId, DefnFuncIds;

  // Store Declarations and Definitions in different maps.
  for (auto const &M: FuncIdMap::getFuncDKeyToId()) {
    // This is definition.
    std::map<FuncId, FuncDeclKey> *TMap = &DeclFuncId;
    if (FuncIdMap::getFuncDeclKeyDecl(M.first)) {
      TMap = &DefnFuncIds;
    }
    (*TMap)[M.second] = M.first;
  }

  O << "{\"FuncInfo\":[\n";

  // Now dump the information for all the interesting functions.
  bool AddComma = false;
  for (auto &NS: this->FuncIDNSArgs) {
    if (AddComma) {
      O << ",\n";
    }
    O << "{\"ID\":" << NS.first;
    O << ",\"TargetParms\":[";
    bool AddComma1 = false;
    for (auto ID: NS.second) {
      if (AddComma1) {
        O << ",";
      }
      O << ID;
      AddComma1 = true;
    }
    O << "]";
    O << ",\"Decl\":{";
    if (DeclFuncId.find(NS.first) != DeclFuncId.end()) {
      O << "\"Name\":\"";
      O << FuncIdMap::getFuncDeclKeyFuncName(DeclFuncId[NS.first]);
      O << "\"";
      O << ",\"FileName\":\"";
      O << FuncIdMap::getFuncDeclKeyFileName(DeclFuncId[NS.first]);
      O << "\"";
    }
    O << "},\"Defn\":{";
    if (DefnFuncIds.find(NS.first) != DefnFuncIds.end()) {
      O << "\"Name\":\"";
      O << FuncIdMap::getFuncDeclKeyFuncName(DefnFuncIds[NS.first]);
      O << "\"";
      O << ",\"FileName\":\"";
      O << FuncIdMap::getFuncDeclKeyFileName(DefnFuncIds[NS.first]);
      O << "\"";
    }
    O << "}";

    O<< "}";
    AddComma = true;
  }
  O << "]}";
}