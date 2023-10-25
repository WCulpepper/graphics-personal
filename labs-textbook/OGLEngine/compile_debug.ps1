Write-Host "Executing compilation command...";
cmake -G "MinGW Makefiles" . -B build -DCMAKE_BUILD_TYPE=Debug;
Set-Location .\build;
make;
Set-Location ..;
