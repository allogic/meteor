#!/bin/bash

project=$1
shader=$2

pushd "projects" > /dev/null 2>&1
	pushd $project > /dev/null 2>&1
		pushd "shaders" > /dev/null 2>&1
            glslc test.vert -o vert.spv
			glslc test.frag -o frag.spv
        popd > /dev/null 2>&1    
	popd > /dev/null 2>&1
popd > /dev/null 2>&1
