#!/bin/bash

project=$1

pushd $project
	pushd "build"
		lldb "$project.exe"
	popd
popd
