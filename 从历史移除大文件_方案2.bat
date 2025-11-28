@echo off
chcp 65001 >nul
echo ========================================
echo 从 Git 历史中移除大文件（方案2）
echo ========================================
echo.
echo 这将使用 git filter-branch 从所有历史记录中移除 .vs 目录
echo.
echo ⚠️  警告：这会重写 Git 历史，需要一些时间
echo.
set /p confirm="确认要继续吗？(Y/N): "
if /i not "%confirm%"=="Y" (
    echo 已取消操作
    pause
    exit /b 0
)

echo.
echo [步骤 1/4] 从历史记录中移除 .vs 目录...
echo 这可能需要几分钟，请耐心等待...
git filter-branch --force --index-filter "git rm -rf --cached --ignore-unmatch .vs" --prune-empty --tag-name-filter cat -- --all
if %ERRORLEVEL% NEQ 0 (
    echo [错误] filter-branch 失败
    pause
    exit /b 1
)
echo ✓ 已从历史中移除
echo.

echo [步骤 2/4] 清理 reflog...
git reflog expire --expire=now --all
echo ✓ reflog 已清理
echo.

echo [步骤 3/4] 清理和压缩仓库...
git gc --prune=now --aggressive
echo ✓ 仓库已清理
echo.

echo [步骤 4/4] 验证 .vs 目录是否已从历史中移除...
git log --all --pretty=format: --name-only --diff-filter=A | findstr /C:".vs" >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [警告] .vs 目录可能仍在历史中
) else (
    echo ✓ .vs 目录已从历史中移除
)
echo.

echo ========================================
echo 完成！
echo ========================================
echo.
echo 现在可以推送:
echo   git push github-ext master --force
echo.
echo ⚠️  注意：需要 --force 推送，因为历史已被重写
echo.
pause

