//=----------DetectERRMain.cpp------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// refverifier tool
//
//===----------------------------------------------------------------------===//

#include "clang/RefVerifier/RefVerifier.h"
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

static cl::OptionCategory RefVerifierCategory("refverifier options");

static const char *HelpOverview =
    "refverifier: Rewrite/verify const references.\n";
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

static cl::opt<bool> OptVerbose("verbose",
                                cl::desc("Print verbose "
                                         "information"),
                                cl::init(false),
                                cl::cat(RefVerifierCategory));

static cl::opt<bool> DumpFuncInfo("dumpfinfo",
                                cl::desc("Dump Function Info"),
                                cl::init(false),
                                  cl::cat(RefVerifierCategory));

static cl::opt<std::string>
    OptFuncInfoJson("funcinfo",
                       cl::desc("Path to the file containing all the function "
                                "info"),
                       cl::init("FuncInfo.json"),
                       cl::cat(RefVerifierCategory));


int main(int argc, const char **argv) {
  struct RefVerifierOptions DOpt;

  sys::PrintStackTraceOnErrorSignal(argv[0]);

  // Initialize targets for clang module support.
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmPrinters();
  InitializeAllAsmParsers();

  // The following code is based on clangTidyMain in
  // clang-tools-extra/clang-tidy/tool/ClangTidyMain.cpp. Apparently every
  // LibTooling-based tool is supposed to duplicate it??
  llvm::Expected<CommonOptionsParser> ExpectedOptionsParser =
      CommonOptionsParser::create(argc, (const char **)(argv), RefVerifierCategory,
                                  cl::ZeroOrMore, HelpOverview);

  if (!ExpectedOptionsParser) {
    llvm::errs() << "refverifier: Error(s) parsing command-line arguments:\n"
                 << llvm::toString(ExpectedOptionsParser.takeError());
    return 1;
  }

  CommonOptionsParser &OptionsParser = *ExpectedOptionsParser;
  // Specifying cl::ZeroOrMore rather than cl::OneOrMore and then checking this
  // here lets us give a better error message than the default "Must specify at
  // least 1 positional argument".
  if (OptionsParser.getSourcePathList().empty()) {
    llvm::errs() << "refverifier: Error: No source files specified.\n"
                 << "See: " << argv[0] << " --help\n";
    return 1;
  }

  // Verbose flag.
  DOpt.Verbose = OptVerbose;

  RefVerifierInterface RefVeriInf(DOpt, OptionsParser.getSourcePathList(),
                             &(OptionsParser.getCompilations()));

  if (DumpFuncInfo) {
    if (RefVeriInf.computeAllFuncInfo()) {
      llvm::outs() << "[+] Successfully computed func info.\n";
    } else {
      llvm::outs() << "[-] Unable to compute func info.\n";
    }
    llvm::outs() << "[+] Trying to write funcinfo to:"
                 << OptFuncInfoJson << ".\n";
    std::error_code Ec;
    llvm::raw_fd_ostream OutputJson(OptFuncInfoJson, Ec);
    if (!OutputJson.has_error()) {
      RefVeriInf.dumpInfo(OutputJson);
      OutputJson.close();
      llvm::outs() << "[+] Finished writing to given output file.\n";
    } else {
      llvm::outs() << "[-] Error trying to open file:" << OptFuncInfoJson << ".\n";
      return -1;
    }
    return 0;
  }


  return 0;
}
