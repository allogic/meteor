$project = $args[0]
$type = $args[1]

$clang = Join-Path (Get-Location) "llvm\build\Release\bin\clang"

$env:CC = $clang
$env:RC = $clang

Push-Location $project
    if (Test-Path -Path "build") {
        Remove-Item -LiteralPath "build" -Force -Recurse
    }
    New-Item -ItemType "directory" -Path "build"
	Push-Location build
        cmake -D"CMAKE_BUILD_TYPE=$type" -G "Unix Makefiles" ..
        cmake --build .
	Pop-Location
Pop-Location
