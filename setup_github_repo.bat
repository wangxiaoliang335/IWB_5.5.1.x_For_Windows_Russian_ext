@echo off
chcp 65001 >nul
echo ========================================
echo 设置GitHub远程仓库
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

echo [1/4] 检查Git仓库状态...
if not exist .git (
    echo 初始化Git仓库...
    git init
    if %ERRORLEVEL% NEQ 0 (
        echo [错误] Git初始化失败
        pause
        exit /b 1
    )
    echo Git仓库初始化成功
) else (
    echo Git仓库已存在
)

echo.
echo [2/4] 查看当前远程仓库配置...
git remote -v

echo.
echo [3/4] 添加新的GitHub远程仓库...
set REMOTE_NAME=github
set REMOTE_URL=git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian.git

REM 检查远程仓库是否已存在
git remote get-url %REMOTE_NAME% >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo 远程仓库 '%REMOTE_NAME%' 已存在，正在更新...
    git remote set-url %REMOTE_NAME% %REMOTE_URL%
) else (
    echo 添加新的远程仓库 '%REMOTE_NAME%'...
    git remote add %REMOTE_NAME% %REMOTE_URL%
)

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 添加远程仓库失败
    pause
    exit /b 1
)

echo.
echo [4/4] 验证远程仓库配置...
git remote -v

echo.
echo ========================================
echo 完成！
echo ========================================
echo.
echo 使用以下命令查看远程仓库:
echo   git remote -v
echo.
echo 使用以下命令推送到GitHub:
echo   git push github main
echo   或
echo   git push github master
echo.
echo 注意: 确保已配置SSH密钥以使用git@github.com地址
echo.
pause

