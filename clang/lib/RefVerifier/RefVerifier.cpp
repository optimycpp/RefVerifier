//=--DetectERR.cpp------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implementation of various method in DetectERR.h
//
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/RefVerifier.h"
#include "clang/RefVerifier/ProjectInfo.h"
#include "clang/RefVerifier/RefVerifierASTConsumer.h"
#include "clang/RefVerifier/RefDiagConsumer.h"
#include "llvm/Support/TargetSelect.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/VerifyDiagnosticConsumer.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "llvm/Support/TargetSelect.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Signals.h"

using namespace clang::driver;
using namespace clang::tooling;
using namespace clang;
using namespace llvm;

template <typename T, typename V, typename W, typename A, typename F>
class GenericAction : public ASTFrontendAction {
public:
  GenericAction(V &I, W &Op, A &Ac, F &FI) : Info(I), Opts(Op), ActT(Ac), FuncI(FI) {}

  virtual std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile) {
    return std::unique_ptr<ASTConsumer>(new T(Info, Opts,
                                              &Compiler.getASTContext(),
                                              ActT, FuncI));
  }

private:
  V &Info;
  W &Opts;
  A &ActT;
  F &FuncI;
};

template <typename T>
std::unique_ptr<FrontendActionFactory>
newFrontendActionFactoryA(ProjectInfo &I, struct RefVerifierOptions &OP,
                          ACTION_ENUM &Ac, FuncId &FI) {
  class ArgFrontendActionFactory : public FrontendActionFactory {
  public:
    explicit ArgFrontendActionFactory(ProjectInfo &I,
                                      struct RefVerifierOptions &OP,
                                      ACTION_ENUM &Ac, FuncId &FI) :
                                      Info(I), Opts(OP), Act(Ac), Fid(FI) {}

    std::unique_ptr<FrontendAction> create() override {
      return std::unique_ptr<FrontendAction>(new T(Info, Opts, Act, Fid));
    }

  private:
    ProjectInfo &Info;
    struct RefVerifierOptions &Opts;
    ACTION_ENUM &Act;
    FuncId &Fid;
  };

  return std::unique_ptr<FrontendActionFactory>(
      new ArgFrontendActionFactory(I, OP, Ac, FI));
}

RefVerifierInterface::RefVerifierInterface(const struct RefVerifierOptions &DEopt,
                     const std::vector<std::string> &SourceFileList,
                     clang::tooling::CompilationDatabase *CompDB) {

  RVeriOptions = DEopt;
  SourceFiles = SourceFileList;
  CurrCompDB = CompDB;
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
}

bool RefVerifierInterface::computeAllFuncInfo() {

  bool RetVal = false;
  auto *Tool = new ClangTool(*CurrCompDB, SourceFiles);
  ACTION_ENUM ACTType = FUNC_INFO;
  FuncId DummyId = 0;

  std::unique_ptr<ToolAction> ConstraintTool = newFrontendActionFactoryA<
      GenericAction<RefVerifierASTConsumer,
                    ProjectInfo, struct RefVerifierOptions,
                    ACTION_ENUM, FuncId>>(this->PInfo,
                                       this->RVeriOptions,
                                       ACTType,
                                        DummyId);

  if (ConstraintTool) {
    Tool->run(ConstraintTool.get());
    RetVal = true;
  } else {
    llvm_unreachable("No action");
  }
  return RetVal;
}

void RefVerifierInterface::dumpInfo(llvm::raw_ostream &O) {
  this->PInfo.dumpFuncIDInfoToJson(O);
}

bool RefVerifierInterface::checkCompilerErrors() {

  bool RetVal = false;
  auto *Tool = new ClangTool(*CurrCompDB, SourceFiles);
  ACTION_ENUM ACTType = FUNC_INFO;
  FuncId DummyId = 0;

  std::unique_ptr<ToolAction> ConstraintTool = newFrontendActionFactoryA<
      GenericAction<RefVerifierASTConsumer,
                    ProjectInfo, struct RefVerifierOptions,
                    ACTION_ENUM, FuncId>>(this->PInfo,
                                          this->RVeriOptions,
                                          ACTType,
                                          DummyId);

  if (ConstraintTool) {
    RefDiagConsumer RDiag(this->PInfo);
    Tool->setDiagnosticConsumer(&RDiag);
    Tool->run(ConstraintTool.get());
    RetVal = true;
  } else {
    llvm_unreachable("No action");
  }
  return RetVal;

}

void RefVerifierInterface::dumpCompileErrors(llvm::raw_ostream &O) {
  // TODO:
}
