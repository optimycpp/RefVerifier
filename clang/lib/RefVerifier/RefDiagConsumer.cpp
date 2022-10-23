//
// Created by machiry on 10/23/22.
//

#include "clang/RefVerifier/RefDiagConsumer.h"

void RefDiagConsumer::HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                     const Diagnostic &Info) {
  // We only care about error.
  if (DiagLevel == clang::DiagnosticsEngine::Error) {
    SourceLocation Loc;
    std::string FileName = "NOFILE";
    unsigned int LineNo = 0;
    if (Info.getLocation().isValid()) {
      // Get line and file number where the diag is.
      Loc = FullSourceLoc(Info.getLocation(), Info.getSourceManager());
      if (Info.hasSourceManager()) {
        const auto &SM = Info.getSourceManager();

        PresumedLoc PL = SM.getPresumedLoc(Loc);
        if (PL.isValid()) {
          FileName = PL.getFilename();
          LineNo = PL.getLine();
        }
      }
    }

    SmallString<64> Message;
    Info.FormatDiagnostic(Message);
    // Add Error message
    PI.addErrorMessage(FileName, LineNo, Message.str().str());
  }
}