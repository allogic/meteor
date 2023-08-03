Push-Location llvm-project
    Remove-Item -LiteralPath "build" -Force -Recurse
    New-Item -ItemType "directory" -Path "build"
	Push-Location build
        & "C:\Program Files\CMake\bin\cmake.exe" -A x64 -D"LLVM_ENABLE_PROJECTS=clang;lld" -D"CMAKE_BUILD_TYPE=Release" -D"CMAKE_INSTALL_PREFIX=C:\Program Files\llvm" -G "Visual Studio 17 2022" ../llvm
        & "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\msbuild.exe" LLVM.sln /p:Configuration=Release
	Pop-Location
Pop-Location