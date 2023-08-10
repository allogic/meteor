#!/bin/bash

project=$1
type=$2

pushd "projects" > /dev/null 2>&1
	pushd $project > /dev/null 2>&1
		rm -rf "build"
		mkdir "build"
		pushd "build" > /dev/null 2>&1
			cmake -D"CMAKE_C_COMPILER=clang" -D"CMAKE_BUILD_TYPE=$type" -G "Unix Makefiles" ".."
			cmake --build "." --parallel 1
		popd > /dev/null 2>&1
	popd > /dev/null 2>&1
popd > /dev/null 2>&1
