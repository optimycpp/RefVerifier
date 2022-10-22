set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_tool(refverifier
   RefVerifierMain.cpp
  )

target_link_libraries(refverifier
  PRIVATE
  clangrefverifier
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  clangTooling
  )

install(TARGETS refverifier
  RUNTIME DESTINATION bin)
