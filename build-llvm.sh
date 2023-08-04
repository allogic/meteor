#!/bin/bash

pushd llvm
	rm -rf build
	mkdir build
	pushd build
		cmake -D"LLVM_ENABLE_PROJECTS=clang;lld" -D"CMAKE_BUILD_TYPE=Release" -D"CMAKE_INSTALL_PREFIX=/opt/llvm" -G "Unix Makefiles" ../llvm
		make -j 8
	popd
popd