Write-Host "Executing compilation command...";
cmake -G "MinGW Makefiles" . -B build;
Set-Location .\build;
make;
Set-Location ..;
