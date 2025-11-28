@echo off
chcp 65001 >nul
echo ========================================
echo 移除大文件并重新推送
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

echo [步骤 1/4] 从 Git 索引中移除 .vs 目录...
git rm -r --cached .vs/ 2>nul
if %ERRORLEVEL% EQU 0 (
    echo ✓ 已从 Git 索引中移除 .vs 目录
) else (
    echo .vs 目录可能不在索引中，继续...
)

echo.
echo [步骤 2/4] 检查 .gitignore 配置...
findstr /C:".vs/" .gitignore >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo ✓ .gitignore 已包含 .vs/ 目录
) else (
    echo [警告] .gitignore 中缺少 .vs/ 目录
    echo 正在添加到 .gitignore...
    echo .vs/ >> .gitignore
    echo ✓ 已添加到 .gitignore
)

echo.
echo [步骤 3/4] 添加更新的文件...
git add .gitignore
if %ERRORLEVEL% EQU 0 (
    echo ✓ 文件已添加
) else (
    echo [错误] 添加文件失败
    pause
    exit /b 1
)

echo.
echo [步骤 4/4] 提交更改...
git commit -m "Remove large files: exclude .vs directory (Visual Studio files)"
if %ERRORLEVEL% EQU 0 (
    echo ✓ 提交成功
) else (
    echo [警告] 提交失败，可能没有更改需要提交
)

echo.
echo ========================================
echo 完成！
echo ========================================
echo.
echo 现在可以执行:
echo   git push github-ext master
echo.
echo 这将推送更改到 GitHub（不包含大文件）
echo.
pause

