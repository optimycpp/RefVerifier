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
#include "json.hpp"

using namespace clang;
using json = nlohmann::json;

void ProjectInfo::addNSArg(const FuncId &FID, int NSArg) {
  this->FuncIDNSArgs[FID].insert(NSArg);
}

void ProjectInfo::addErrorMessage(const std::string &FN,
                                  unsigned int LineNo,
                                  const std::string &Msg) {
  this->ErrMessages[FN].insert(std::make_pair(LineNo, Msg));
}

void updateJsonObj(json &JObj, const FuncDeclKey &FD) {
  JObj["Name"] = FuncIdMap::getFuncDeclKeyFuncName(FD);
  JObj["FileName"] = FuncIdMap::getFuncDeclKeyFileName(FD);
  JObj["LineNum"] = FuncIdMap::getFuncDeclKeyLineNum(FD);
  JObj["EndLineNum"] = FuncIdMap::getFuncDeclKeyEndLineNum(FD);
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

  json Mobj;

  std::vector<json> FuncObjs;

  // Now dump the information for all the interesting functions.
  bool AddComma = false;
  for (auto &NS: this->FuncIDNSArgs) {
    json FObj;
    FObj["ID"] = NS.first;
    FObj["TargetParms"] = NS.second;
    if (DeclFuncId.find(NS.first) != DeclFuncId.end()) {
      updateJsonObj(FObj["Decl"], DeclFuncId[NS.first]);
    }
    if (DefnFuncIds.find(NS.first) != DefnFuncIds.end()) {
      updateJsonObj(FObj["Defn"], DefnFuncIds[NS.first]);
    }
    FuncObjs.push_back(FObj);
  }
  Mobj["FuncInfo"] = FuncObjs;
  O << Mobj.dump(4);
}

void ProjectInfo::dumpErrorInfoToJson(llvm::raw_ostream &O) const {
  O << "{\"ErrorInfo\":[";
  bool AddComma = false;
  for (auto &EI : this->ErrMessages) {
    if (AddComma)
      O << ",\n";
    O << "{\"FileName\":\"";
    O << EI.first << "\", \"Errs\":[";

    bool AddComma1 = false;
    for (auto &ES : EI.second) {
      if (AddComma1)
        O << ",";
      O << "{\"LineNo\":";
      O << ES.first;
      O << ", \"Message\":\"";
      O << ES.second;
      O << "\"}";
      AddComma1 = true;
    }
    O << "]}";
    AddComma = true;
  }
  O << "]}";

}