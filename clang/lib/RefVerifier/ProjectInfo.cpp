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
#include "clang/RefVerifier/json.hpp"
#include <fstream>

using namespace clang;
using json = nlohmann::json;

void ProjectInfo::addNSArg(const FuncId &FID, int NSArg,
                           const ParmVarDecl *PVD) {
  ParamRewriteKey PKey(PVD);
  this->FuncIDNSArgs[FID][NSArg].insert(PKey);
}

void ProjectInfo::addErrorMessage(const std::string &FN,
                                  unsigned int LineNo,
                                  const std::string &Msg) {
  this->ErrMessages[FN].insert(std::make_pair(LineNo, Msg));
}

bool ProjectInfo::parseFuncIDInfoJson(const std::string &JsonFP) {
  std::ifstream FStream(JsonFP);
  bool RVal = false;
  if (FStream) {
    json Obj = json::parse(FStream);
    if (Obj.contains("FuncInfo")) {
      for (const auto &FObj : Obj["FuncInfo"]) {
        const FuncId &FID = FObj["ID"];
        if (FObj.contains("Decl")) {
          FuncIdMap::addFuncDecl(FObj["Decl"], FID);
        }
        if (FObj.contains("Defn")) {
          FuncIdMap::addFuncDecl(FObj["Defn"], FID);
        }
      }
    }
    RVal = true;
  } else {
    llvm::outs() << "[-] Failed to parse provided json:" << JsonFP << "\n";
  }
  return RVal;
}

void ProjectInfo::dumpFuncIDInfoToJson(llvm::raw_ostream &O) const {
  std::map<FuncId, FuncDeclKey> DeclFuncId, DefnFuncIds;

  // Store Declarations and Definitions in different maps.
  for (auto const &M: FuncIdMap::getFuncDKeyToId()) {
    // This is definition.
    std::map<FuncId, FuncDeclKey> *TMap = &DeclFuncId;
    if (M.first.IsDefinition) {
      TMap = &DefnFuncIds;
    }
    (*TMap)[M.second] = M.first;
  }

  json Mobj;

  std::vector<json> FuncObjs;

  // Now dump the information for all the interesting functions.
  for (auto &NS: this->FuncIDNSArgs) {
    json FObj;
    FObj["ID"] = NS.first;
    json PObj;
    for (const auto &E : NS.second) {
      PObj[std::to_string(E.first)] = E.second;
    }
    FObj["NonConstNonScalarParams"] = PObj;
    if (DeclFuncId.find(NS.first) != DeclFuncId.end()) {
      FObj["Decl"] = DeclFuncId[NS.first];
    }
    if (DefnFuncIds.find(NS.first) != DefnFuncIds.end()) {
      FObj["Defn"] = DefnFuncIds[NS.first];
    }
    FuncObjs.push_back(FObj);
  }
  Mobj["FuncInfo"] = FuncObjs;
  O << Mobj.dump(4);
}

void ProjectInfo::dumpErrorInfoToJson(llvm::raw_ostream &O) const {
  json Eobj;
  std::vector<json> EMsgs;
  for (auto &EI : this->ErrMessages) {
    json FileMsgs;
    FileMsgs["FileName"] = EI.first;
    std::vector<json> Msgs;
    Msgs.clear();
    for (auto &ES : EI.second) {
      json Msg;
      Msg["LineNo"] = ES.first;
      Msg["Message"] = ES.second;
      Msgs.push_back(Msg);
    }
    FileMsgs["Errs"] = Msgs;
    EMsgs.push_back(FileMsgs);
  }
  Eobj["ErrorInfo"] = EMsgs;
  O << Eobj.dump(4);
}