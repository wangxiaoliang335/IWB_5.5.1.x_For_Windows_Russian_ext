@echo off
chcp 65001 >nul
echo ========================================
echo 取消 IWB 子模块设置并重新提交
echo ========================================
echo.

REM 检查Git是否已安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到Git，请先安装Git
    pause
    exit /b 1
)

echo [步骤 1/6] 移除子模块配置...
if exist .git\modules\IWB (
    rmdir /s /q .git\modules\IWB
    echo ✓ 已移除 .git/modules/IWB
) else (
    echo .git/modules/IWB 不存在
)

REM 从 .git/config 中移除子模块配置
git config -f .git/config --remove-section submodule.IWB 2>nul
if %ERRORLEVEL% EQU 0 (
    echo ✓ 已从 .git/config 移除子模块配置
) else (
    echo .git/config 中没有子模块配置
)

if exist .gitmodules (
    del /f .gitmodules 2>nul
    echo ✓ 已删除 .gitmodules 文件
) else (
    echo .gitmodules 文件不存在
)
echo.

echo [步骤 2/6] 从 Git 索引中移除 IWB 子模块...
git rm --cached IWB 2>nul
if %ERRORLEVEL% EQU 0 (
    echo ✓ IWB 已从索引中移除
) else (
    echo IWB 可能不在索引中，继续...
)
echo.

echo [步骤 3/6] 移除 IWB 的 .git 目录...
if exist IWB\.git (
    rmdir /s /q IWB\.git
    echo ✓ IWB/.git 目录已移除
) else (
    echo IWB/.git 目录不存在
)
echo.

echo [步骤 4/6] 确保 IWB/.git/ 在 .gitignore 中...
findstr /C:"IWB/.git/" .gitignore >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo IWB/.git/ >> .gitignore
    echo ✓ 已添加到 .gitignore
) else (
    echo ✓ IWB/.git/ 已在 .gitignore 中
)
echo.

echo [步骤 5/6] 重新添加 IWB 作为普通目录...
git add IWB
if %ERRORLEVEL% EQU 0 (
    echo ✓ IWB 目录已添加
) else (
    echo [错误] 添加 IWB 失败
    pause
    exit /b 1
)
echo.

echo [步骤 6/6] 提交更改...
git commit -m "Convert IWB from submodule to regular directory"
if %ERRORLEVEL% EQU 0 (
    echo ✓ 已提交
) else (
    echo [错误] 提交失败
    pause
    exit /b 1
)
echo.

echo ========================================
echo 完成！
echo ========================================
echo.
echo 现在可以推送到 GitHub:
echo   git push github-ext master
echo.
pause

