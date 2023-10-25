Write-Host "Debugging program...";
Set-Location .\build\src;
gdb OGLEngine;
Set-Location ..;
Set-Location ..;