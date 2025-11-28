@echo off
chcp 65001 >nul
echo ========================================
echo 添加新的 GitHub 远程仓库
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

set NEW_REMOTE_NAME=github-ext
set NEW_REMOTE_URL=git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian_ext.git

echo [1/5] 查看当前远程仓库配置...
git remote -v
echo.

echo [2/5] 添加新的远程仓库...
echo 远程名称: %NEW_REMOTE_NAME%
echo 远程地址: %NEW_REMOTE_URL%
echo.

REM 检查远程仓库是否已存在
git remote get-url %NEW_REMOTE_NAME% >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo 远程仓库 '%NEW_REMOTE_NAME%' 已存在，正在更新...
    git remote set-url %NEW_REMOTE_NAME% %NEW_REMOTE_URL%
) else (
    echo 添加新的远程仓库 '%NEW_REMOTE_NAME%'...
    git remote add %NEW_REMOTE_NAME% %NEW_REMOTE_URL%
)

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 添加远程仓库失败
    pause
    exit /b 1
)

echo ✓ 远程仓库已添加/更新
echo.

echo [3/5] 验证远程仓库配置...
git remote -v
echo.

echo [4/5] 检查 .gitignore 配置...
if exist .gitignore (
    echo ✓ .gitignore 文件存在
    findstr /C:"*.zip" .gitignore >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo ✓ 已配置排除压缩文件
    ) else (
        echo [警告] .gitignore 中可能没有配置排除大文件
    )
) else (
    echo [警告] .gitignore 文件不存在
)
echo.

echo [5/5] 下一步操作...
echo ========================================
echo 完成！新的远程仓库已添加。
echo ========================================
echo.
echo 接下来您可以执行:
echo   1. git status              - 查看状态
echo   2. git add .               - 添加文件（会自动排除大于50M的文件）
echo   3. git commit -m "Your message"  - 提交更改
echo   4. git push %NEW_REMOTE_NAME% master  - 推送到新仓库
echo.
echo 如果遇到 SSH 连接问题，可以改用 HTTPS:
echo   git remote set-url %NEW_REMOTE_NAME% https://github.com/wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian_ext.git
echo.
pause

