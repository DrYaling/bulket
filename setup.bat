set RUST_ROOT=%cd%
mkdir "..\ProjectU\Binaries"
cd "..\ProjectU\Source\ProjectU"
mklink /D "RustGamePlay" "%RUST_ROOT%\unreal\Source\RustGamePlay"
mklink /D "ThirdPerson" "..\..\projectK\unreal\Content\ThirdPerson"
pause