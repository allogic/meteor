Push-Location llvm
    if (Test-Path -Path "build") {
        Remove-Item -LiteralPath "build" -Force -Recurse
    }
    New-Item -ItemType "directory" -Path "build"
	Push-Location build
        cmake -D"LLVM_ENABLE_PROJECTS=clang;lld;lldb" -D"CMAKE_BUILD_TYPE=Release" -G "Visual Studio 17 2022" ../llvm
        #cmake --build .
        msbuild LLVM.sln /p:Configuration=Release
        Copy-Item -Path "Release\bin" -Destination "..\..\clang" -Recurse
	Pop-Location
Pop-Location
