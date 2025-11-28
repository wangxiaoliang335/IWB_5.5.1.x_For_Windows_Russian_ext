@echo off
chcp 65001 >nul
echo ========================================
echo 解决 IWB 子模块问题
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

echo [选项 1] 移除 IWB 的 .git 目录，然后添加 IWB 内容（推荐）
echo [选项 2] 完全忽略 IWB 目录
echo.
set /p choice="请选择 (1 或 2): "

if "%choice%"=="1" (
    echo.
    echo [步骤 1/4] 移除 IWB 的 .git 目录...
    if exist IWB\.git (
        rmdir /s /q IWB\.git
        echo ✓ IWB/.git 目录已移除
    ) else (
        echo IWB/.git 目录不存在，继续...
    )
    echo.
    
    echo [步骤 2/4] 添加 IWB 目录...
    git add IWB
    if %ERRORLEVEL% EQU 0 (
        echo ✓ IWB 目录已添加
    ) else (
        echo [错误] 添加失败
        pause
        exit /b 1
    )
    echo.
    
    echo [步骤 3/4] 提交更改...
    git commit -m "Add IWB directory (removed nested .git)"
    if %ERRORLEVEL% EQU 0 (
        echo ✓ 已提交
    ) else (
        echo [错误] 提交失败
        pause
        exit /b 1
    )
    echo.
    
    echo [步骤 4/4] 推送到 GitHub...
    git push github-ext master
    echo.
    echo ✓ 完成！
    
) else if "%choice%"=="2" (
    echo.
    echo [步骤 1/4] 从 Git 跟踪中移除 IWB...
    git rm --cached IWB 2>nul
    echo ✓ IWB 已从跟踪中移除
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
) else (
    echo 无效的选择
    pause
    exit /b 1
)

echo.
pause

