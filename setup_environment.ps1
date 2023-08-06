$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin"
$clang = "$(Get-Location)\clang"

$env:PATH += ";$clang"
$env:PATH += ";$msbuild"
