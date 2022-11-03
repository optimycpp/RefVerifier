#!/bin/bash
mkdir projects
cd /projects/
GIT_SSL_NO_VERIFY=true git clone https://github.com/optimycpp/RefVerifier.git
cd RefVerifier
mkdir build
cd build
cmake ../llvm -G Ninja -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD=X86 -DLLVM_USE_SPLIT_DWARF=ON -DLLVM_OPTIMIZED_TABLEGEN=ON
ninja refverifier clang
echo "export PATH=/projects/RefVerifier/build/bin:$PATH" >> ~/.bashrc
