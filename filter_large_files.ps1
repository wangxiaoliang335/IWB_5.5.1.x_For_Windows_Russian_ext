# PowerShell脚本：找出并配置忽略大于50M的文件

Write-Host "扫描大于50M的文件..." -ForegroundColor Green

$largeFiles = Get-ChildItem -Path . -Recurse -File | Where-Object { 
    $_.Length -gt 50MB -and $_.FullName -notlike "*\.git\*"
} | Select-Object FullName, @{Name="Size(MB)";Expression={[math]::Round($_.Length / 1MB, 2)}}

if ($largeFiles) {
    Write-Host "`n找到以下大于50M的文件:" -ForegroundColor Yellow
    $largeFiles | Format-Table FullName, "Size(MB)" -AutoSize
    
    # 生成.gitignore条目
    $gitignoreEntries = $largeFiles | ForEach-Object {
        # 转换为相对于仓库根目录的路径
        $relativePath = $_.FullName.Replace((Get-Location).Path + "\", "").Replace("\", "/")
        $relativePath
    }
    
    Write-Host "`n这些文件将被添加到.gitignore中..." -ForegroundColor Yellow
    
    # 读取或创建.gitignore文件
    if (Test-Path .gitignore) {
        $existingContent = Get-Content .gitignore -Raw
    } else {
        $existingContent = ""
    }
    
    # 添加大文件忽略规则
    $newContent = "# 忽略大于50M的文件`n"
    $newContent += "# 自动生成于 $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`n`n"
    
    foreach ($entry in $gitignoreEntries) {
        if ($existingContent -notmatch [regex]::Escape($entry)) {
            $newContent += "$entry`n"
        }
    }
    
    # 追加到.gitignore
    if ($existingContent) {
        Add-Content -Path .gitignore -Value "`n$newContent"
    } else {
        Set-Content -Path .gitignore -Value $newContent
    }
    
    Write-Host "`.gitignore 已更新" -ForegroundColor Green
} else {
    Write-Host "`n没有找到大于50M的文件" -ForegroundColor Green
}

Write-Host "`n完成！" -ForegroundColor Green

