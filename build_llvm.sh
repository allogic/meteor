#!/bin/bash

pushd "llvm" > /dev/null 2>&1
	rm -rf "build"
	mkdir "build"
	pushd "build" > /dev/null 2>&1
		cmake -D"LLVM_ENABLE_PROJECTS=clang;lld;lldb" -D"CMAKE_BUILD_TYPE=Release" -G "Unix Makefiles" "../llvm"
		cmake --build "." --parallel 8
		cp -R "bin" "../../bin"
	popd
popd
