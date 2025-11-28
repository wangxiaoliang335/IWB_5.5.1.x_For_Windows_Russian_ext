@echo off
chcp 65001 >nul
echo ========================================
echo 重新初始化仓库并推送（移除大文件历史）
echo ========================================
echo.
echo ⚠️  警告：这将删除当前的 Git 历史记录
echo    但会保留所有本地文件
echo.
set /p confirm="确认要继续吗？(Y/N): "
if /i not "%confirm%"=="Y" (
    echo 已取消操作
    pause
    exit /b 0
)

echo.
echo [步骤 1/6] 备份当前远程仓库配置...
git remote -v > remote_backup.txt
echo ✓ 远程配置已备份到 remote_backup.txt
echo.

echo [步骤 2/6] 移除 .git 目录...
if exist .git (
    rmdir /s /q .git
    echo ✓ .git 目录已移除
) else (
    echo .git 目录不存在
)
echo.

echo [步骤 3/6] 重新初始化 Git 仓库...
git init
if %ERRORLEVEL% NEQ 0 (
    echo [错误] Git 初始化失败
    pause
    exit /b 1
)
echo ✓ Git 仓库已重新初始化
echo.

echo [步骤 4/6] 添加远程仓库...
git remote add github-ext git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian_ext.git
if %ERRORLEVEL% NEQ 0 (
    echo 尝试更新远程仓库...
    git remote set-url github-ext git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian_ext.git
)
echo ✓ 远程仓库已配置
echo.

echo [步骤 5/6] 检查 .gitignore 配置...
findstr /C:".vs/" .gitignore >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo ✓ .gitignore 已包含 .vs/ 目录
) else (
    echo [警告] .gitignore 中缺少 .vs/ 目录，正在添加...
    echo .vs/ >> .gitignore
)
echo.

echo [步骤 6/6] 添加文件（将自动排除 .vs/ 和大文件）...
echo 正在添加文件，请稍候...
git add .
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 添加文件失败
    pause
    exit /b 1
)
echo ✓ 文件已添加
echo.

echo ========================================
echo 完成！
echo ========================================
echo.
echo 下一步操作:
echo   1. git commit -m "Initial commit: IWB 5.5.1.x Russian version"
echo   2. git push github-ext master
echo.
echo 这将创建一个全新的提交，不包含大文件的历史记录。
echo.
pause

