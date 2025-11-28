@echo off
chcp 65001 >nul
echo ========================================
echo 处理 IWB 目录
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

echo [选项 1] 完全忽略 IWB 目录（推荐）
echo [选项 2] 添加 IWB 目录但排除 .git
echo.
set /p choice="请选择 (1 或 2): "

if "%choice%"=="1" (
    echo.
    echo [步骤 1/4] 从 Git 跟踪中移除 IWB 目录...
    git rm --cached -r IWB
    echo ✓ IWB 目录已从跟踪中移除
    echo.
    
    echo [步骤 2/4] 添加到 .gitignore...
    findstr /C:"^IWB/$" .gitignore >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo IWB/ >> .gitignore
        echo ✓ 已添加到 .gitignore
    ) else (
        echo ✓ IWB/ 已在 .gitignore 中
    )
    echo.
    
    echo [步骤 3/4] 提交更改...
    git add .gitignore
    git commit -m "Ignore IWB directory (nested git repository)"
    echo ✓ 已提交
    echo.
    
    echo [步骤 4/4] 推送到 GitHub...
    git push github-ext master
    echo.
    echo ✓ 完成！
    
) else if "%choice%"=="2" (
    echo.
    echo [步骤 1/3] 确保 IWB/.git/ 在 .gitignore 中...
    findstr /C:"IWB/.git/" .gitignore >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo IWB/.git/ >> .gitignore
        echo ✓ 已添加到 .gitignore
    ) else (
        echo ✓ 已在 .gitignore 中
    )
    echo.
    
    echo [步骤 2/3] 添加 IWB 目录（排除 .git）...
    git add IWB
    git commit -m "Add IWB directory (excluding .git)"
    echo ✓ 已提交
    echo.
    
    echo [步骤 3/3] 推送到 GitHub...
    git push github-ext master
    echo.
    echo ✓ 完成！
) else (
    echo 无效的选择
    pause
    exit /b 1
)

echo.
pause

