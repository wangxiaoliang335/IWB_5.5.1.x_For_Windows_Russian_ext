# GitHub 仓库设置说明

## 已完成的工作

1. ✅ 创建了 `.gitignore` 文件，用于排除：
   - 构建产物目录（bin/, Debug/, Release/ 等）
   - 中间文件（*.obj, *.pdb, *.ilk 等）
   - 大型压缩文件（*.zip, *.7z 等）
   - 可执行文件和动态库（*.exe, *.dll 等）

2. ✅ 创建了 `setup_github_repo.bat` 脚本用于：
   - 检查并初始化 Git 仓库（如果还没有）
   - 添加新的 GitHub 远程仓库
   - 配置远程仓库为：git@github.com:wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian.git

3. ✅ 创建了 `check_large_files.bat` 脚本用于检查大于 50M 的文件

## 使用步骤

### 步骤 1: 运行设置脚本
双击运行 `setup_github_repo.bat`，或在命令行中执行：
```batch
setup_github_repo.bat
```

这将：
- 初始化 Git 仓库（如果还没有）
- 添加名为 `github` 的远程仓库指向您的新 GitHub 仓库

### 步骤 2: 检查大文件
运行 `check_large_files.bat` 来找出大于 50M 的文件：
```batch
check_large_files.bat
```

### 步骤 3: 配置 Git 用户信息（如果还没有配置）
```batch
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

### 步骤 4: 添加文件并提交
```batch
git add .
git commit -m "Initial commit: IWB 5.5.1.x Russian version"
```

### 步骤 5: 推送到 GitHub
```batch
git push github main
```
或
```batch
git push github master
```

## 关于大文件过滤

`.gitignore` 文件已经配置为排除常见的大文件和构建产物。如果仍然有大文件需要排除：

1. 运行 `check_large_files.bat` 找出大文件
2. 将这些文件路径添加到 `.gitignore` 文件中

## 重要提示

- 确保您已经配置了 SSH 密钥以使用 `git@github.com` 地址
- 如果还没有配置 SSH，可以使用 HTTPS 地址：`https://github.com/wangxiaoliang335/IWB_5.5.1.x_For_Windows_Russian.git`
- GitHub 有文件大小限制（100MB），超过 50MB 的文件建议使用 Git LFS 或排除在仓库之外

## 使用不同的远程名称

如果您的本地已经有一个名为 `origin` 的远程仓库，新添加的远程仓库将使用名称 `github`。您可以使用以下命令查看所有远程仓库：

```batch
git remote -v
```

## 常见问题

**Q: 如何修改远程仓库地址？**
A: 使用 `git remote set-url github <new-url>`

**Q: 如何删除远程仓库？**
A: 使用 `git remote remove github`

**Q: 如何检查哪些文件会被提交？**
A: 使用 `git status` 查看暂存区的文件

