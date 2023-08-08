$project = $args[0]
$type = $args[1]

Push-Location $project
	if (Test-Path -Path "build") {
		Remove-Item -LiteralPath "build" -Force -Recurse
	}
	New-Item -ItemType "directory" -Path "build" *> $null
	Push-Location build
		cmake -D"CMAKE_C_COMPILER=clang" -D"CMAKE_BUILD_TYPE=$type" -G "Unix Makefiles" "..\llvm"
		cmake --build "." --parallel 8
	Pop-Location
Pop-Location
