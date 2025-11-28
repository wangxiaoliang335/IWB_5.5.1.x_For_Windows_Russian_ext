@echo off
chcp 65001 >nul
echo ========================================
echo 完成提交和推送
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

echo [步骤 1/5] 确保 .gitignore 包含 IWB/.git/...
findstr /C:"IWB/.git/" .gitignore >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo 正在添加 IWB/.git/ 到 .gitignore...
    echo IWB/.git/ >> .gitignore
    echo ✓ 已添加到 .gitignore
) else (
    echo ✓ .gitignore 已包含 IWB/.git/
)
echo.

echo [步骤 2/5] 禁用嵌入仓库警告...
git config advice.addEmbeddedRepo false
echo ✓ 警告已禁用
echo.

echo [步骤 3/5] 添加所有文件...
git add .
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 添加文件失败
    pause
    exit /b 1
)
echo ✓ 文件已添加
echo.

echo [步骤 4/5] 提交更改...
git commit -m "Initial commit: IWB 5.5.1.x Russian version"
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 提交失败
    pause
    exit /b 1
)
echo ✓ 提交成功
echo.

echo [步骤 5/5] 推送到 GitHub...
echo 正在推送到 github-ext...
git push github-ext master
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo ✓ 推送成功！
    echo ========================================
) else (
    echo.
    echo [错误] 推送失败
    echo 请检查网络连接和远程仓库配置
)

echo.
pause

