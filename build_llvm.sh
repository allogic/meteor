#!/bin/bash

pushd llvm
	rm -rf build
	mkdir build
	pushd build
		cmake -D"LLVM_ENABLE_PROJECTS=clang;lld;lldb" -D"CMAKE_BUILD_TYPE=Release" -G "Unix Makefiles" ../llvm
		make
	popd
popd
