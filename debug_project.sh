#!/bin/bash

project=$1

pushd $project > /dev/null 2>&1
	pushd "build" > /dev/null 2>&1
		lldb "$project.exe"
	popd
popd
