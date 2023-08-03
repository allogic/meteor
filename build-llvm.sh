#!/bin/sh

pushd llvm-project
	rm -rf build
	mkdir build
	pushd build
		cmake -A x64 -D"LLVM_ENABLE_PROJECTS=clang;lld" -D"CMAKE_BUILD_TYPE=Release" -D"CMAKE_INSTALL_PREFIX=/opt/llvm" -G "Unix Makefiles" ../llvm
		make
	popd
popd