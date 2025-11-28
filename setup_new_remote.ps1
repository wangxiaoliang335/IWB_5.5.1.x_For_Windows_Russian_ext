# PowerShell脚本：添加新的GitHub远程仓库并配置只提交小于50M的文件

Write-Host "检查Git仓库状态..." -ForegroundColor Green

# 检查是否已初始化Git仓库
if (-not (Test-Path .git)) {
    Write-Host "初始化Git仓库..." -ForegroundColor Yellow
    git init
}

# 检查当前远程仓库
Write-Host "`n当前远程仓库配置:" -ForegroundColor Green
git remote -v

# 添加新的远程仓库（使用github作为远程名称）
$newRemoteName = "github"
$newRemoteUrl = "git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian.git"

Write-Host "`n添加新的远程仓库: $newRemoteName -> $newRemoteUrl" -ForegroundColor Yellow
git remote add $newRemoteName $newRemoteUrl

# 验证远程仓库已添加
Write-Host "`n更新后的远程仓库配置:" -ForegroundColor Green
git remote -v

Write-Host "`n完成！新的远程仓库已添加。" -ForegroundColor Green
Write-Host "使用 'git remote -v' 查看所有远程仓库" -ForegroundColor Cyan
Write-Host "使用 'git push $newRemoteName <branch-name>' 推送到新仓库" -ForegroundColor Cyan

