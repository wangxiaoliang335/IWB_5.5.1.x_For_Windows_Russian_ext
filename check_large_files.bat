@echo off
chcp 65001 >nul
echo ========================================
echo 检查大于50M的文件
echo ========================================
echo.

REM PowerShell脚本检查大文件
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
    "$largeFiles = Get-ChildItem -Path . -Recurse -File -ErrorAction SilentlyContinue | Where-Object { $_.Length -gt 50MB -and $_.FullName -notlike '*\.git\*' -and $_.FullName -notlike '*\bin\*' -and $_.FullName -notlike '*\Debug\*' -and $_.FullName -notlike '*\Release\*' -and $_.FullName -notlike '*\Intermediate\*' -and $_.FullName -notlike '*\Intermidate\*' }; ^
    if ($largeFiles) { ^
        Write-Host '找到以下大于50M的文件:' -ForegroundColor Yellow; ^
        $largeFiles | ForEach-Object { Write-Host (''{0}'' -f $_.FullName) -ForegroundColor Red; Write-Host ('  大小: {0:N2} MB' -f ($_.Length / 1MB)) -ForegroundColor Gray }; ^
        Write-Host ''; ^
        Write-Host '这些文件应该被添加到.gitignore中' -ForegroundColor Yellow; ^
        Write-Host '或者考虑使用Git LFS来管理大文件' -ForegroundColor Cyan; ^
    } else { ^
        Write-Host '未找到大于50M的文件（已排除构建目录）' -ForegroundColor Green; ^
    }"

echo.
echo ========================================
echo 完成！
echo ========================================
echo.
pause

